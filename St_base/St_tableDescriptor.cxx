//*-- Author :    Valery Fine   09/08/99  (E-mail: fine@bnl.gov)
// $Id: St_tableDescriptor.cxx,v 1.6 1999/12/03 20:51:27 fine Exp $
// $Log: St_tableDescriptor.cxx,v $
// Revision 1.6  1999/12/03 20:51:27  fine
// change the data-member name size from 20 to 32 to be in line with dstype.h
//
// Revision 1.5  1999/09/13 13:28:27  fine
// One cast int -> uint introduced to avoid compiler warnings
//
// Revision 1.4  1999/08/13 16:35:53  fine
// The artificial restrictions for tableDescriptor have been introduced since database want that
//
// Revision 1.3  1999/08/12 18:53:49  fine
// clash between St_tableDescriptor::GetSize and St_Table::GetSize resolved
//
// Revision 1.2  1999/08/11 14:44:39  fine
// name clash with ROOT over enum resolved
//
// Revision 1.1  1999/08/11 00:40:11  fine
// new class St_tableDescriptor
//

#include <stdlib.h> 
#include "St_tableDescriptor.h"
#include "St_Table.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "Stypes.h"                       

St_tableDescriptor *St_tableDescriptor::fgColDescriptors = 0; 
TableImp(tableDescriptor)                     

//______________________________________________________________________________
void St_tableDescriptor::Streamer(TBuffer &R__b) 
{
  // This class needs a special version of Streamer because of pointers
  if (!R__b.IsReading()) R__b.WriteVersion(St_tableDescriptor::IsA());
  St_Table::Streamer(R__b);
}                                             
//______________________________________________________________________________
St_tableDescriptor::St_tableDescriptor(const St_Table *parentTable)
 : St_Table("tableDescriptor",sizeof(tableDescriptor_st))
{ 
  SetType("tableDescriptor");
  if (parentTable) {
      SetName(parentTable->GetName());
      LearnTable(parentTable); 
  }  
}
//______________________________________________________________________________
St_tableDescriptor::~St_tableDescriptor()
{
  if (!IsZombie()) {
    for (Int_t i=0;i<GetNRows();i++) {
      Char_t *name = (Char_t *)GetColumnName(i);
      if (name) delete [] name; 
      UInt_t  *indxArray = (UInt_t *)GetIndexArray(i);
      if (indxArray) delete [] indxArray; 
    }
  }
}
//____________________________________________________________________________
void St_tableDescriptor::LearnTable(const St_Table *parentTable)
{
//
//  LearnTable() creates an array of the descriptors for elements of the row
//
//  This is to introduce an artificial restriction demanded by STAR database group
//
//    1. the name may be 19 symbols at most
//    2. the number the dimension is 2 at most
//
//  To lift this restriction one has to provide -DNORESTRICTIONS CPP symbol and
//  recompile code.
//
  if (!parentTable) {
    MakeZombie();
    return;
  }
 
  TClass *classPtr = parentTable->GetRowClass();
  if (!classPtr) return;

  if (!classPtr->GetListOfRealData()) classPtr->BuildRealData();
  if (!(classPtr->GetNdata())) return;

  const Char_t *types;
  Char_t *varname;

  tableDescriptor_st elementDescriptor;

  ReAllocate(classPtr->GetListOfDataMembers()->GetSize());
  Int_t columnIndex = 0;
  TIter next(classPtr->GetListOfDataMembers());
  TDataMember *member = 0;
  while ( (member = (TDataMember *) next()) ) {
    memset(&elementDescriptor,0,sizeof(tableDescriptor_st));
    varname = (Char_t *) member->GetName();
#ifdef NORESTRICTIONS
//  This is remove to introduce an artificial restriction demanded by STAR infrastructure group
                                             elementDescriptor.m_ColumnName = StrDup(varname);
#else
                                             elementDescriptor.m_ColumnName[0] = '\0';
         strncat(elementDescriptor.m_ColumnName,varname,sizeof(elementDescriptor.m_ColumnName));
#endif
    // define index
    TDataType *memberType = member->GetDataType();
                                              elementDescriptor.m_TypeSize = memberType->Size(); 
    types = memberType->GetTypeName();
    elementDescriptor.m_Type = kNAN;
    if (!strcmp("float", types))              elementDescriptor.m_Type = kFloat;  
    else if (!strcmp("int", types))           elementDescriptor.m_Type = kInt;  
    else if (!strcmp("long", types))          elementDescriptor.m_Type = kLong;  
    else if (!strcmp("short", types))         elementDescriptor.m_Type = kShort;  
    else if (!strcmp("double", types))        elementDescriptor.m_Type = kDouble;  
    else if (!strcmp("unsigned int", types))  elementDescriptor.m_Type = kUInt;  
    else if (!strcmp("unsigned long", types)) elementDescriptor.m_Type = kULong ; 
    else if (!strcmp("unsigned short", types))elementDescriptor.m_Type = kUShort; 
    else if (!strcmp("unsigned char", types)) elementDescriptor.m_Type = kUChar;
    else if (!strcmp("char", types))          elementDescriptor.m_Type = kChar;

    Int_t globalIndex = 1;
    if (elementDescriptor.m_Type != kNAN) {
      Int_t dim = 0;
      if ( (dim = member->GetArrayDim()) ) {
                                              elementDescriptor.m_Dimensions = dim;
#ifdef NORESTRICTIONS
                                              elementDescriptor.m_IndexArray = new UInt_t(dim);
#else
       if (UInt_t(dim) > sizeof(elementDescriptor.m_IndexArray)/sizeof(UInt_t *)) {
                Error("LearnTable","Too many dimenstions - %d", dim);
                dim =  2;
       }
#endif
        for( Int_t indx=0; indx < dim; indx++ ){
                                              elementDescriptor.m_IndexArray[indx] = member->GetMaxIndex(indx);
          globalIndex *= elementDescriptor.m_IndexArray[indx];
        }
      } 
    }
    else Error("LearnTable","Wrong data type for <%s> table",parentTable->GetType());
    elementDescriptor.m_Size   =  globalIndex * (elementDescriptor.m_TypeSize);
    elementDescriptor.m_Offset = member->GetOffset();
    AddAt(&elementDescriptor,columnIndex); columnIndex++;
  }
}
//____________________________________________________________________________
const Int_t St_tableDescriptor::GetColumnByName(const Char_t *columnName) const 
{ 
 const tableDescriptor_st *elementDescriptor = ((St_tableDescriptor *)this)->GetTable();
 Int_t i = -1;
 if (!elementDescriptor) return i;
 Int_t nRows = GetNRows();
 if (nRows) {
   for (Int_t i=0; i < nRows; i++)
     if (strcmp(columnName,elementDescriptor->m_ColumnName) == 0) break;
 }
 if (i==nRows) i = -1;
 return i;
}
//____________________________________________________________________________
Int_t St_tableDescriptor::GetOffset(const Char_t *columnName) const 
{  
  Int_t indx = GetColumnByName(columnName);
  if (indx >= 0 ) indx = GetOffset(indx);
  return indx;
}
//____________________________________________________________________________
Int_t St_tableDescriptor::GetColumnSize(const Char_t *columnName) const 
{ 
  Int_t indx = GetColumnByName(columnName);
  if (indx >= 0 ) indx = GetColumnSize(indx);
  return indx;
}
//____________________________________________________________________________
Int_t St_tableDescriptor::GetTypeSize(const Char_t *columnName) const 
{
  Int_t indx = GetColumnByName(columnName);
  if (indx >= 0 ) indx = GetTypeSize(indx);
  return indx;
}
//____________________________________________________________________________
Int_t St_tableDescriptor::GetDimensions(const Char_t *columnName) const 
{
  Int_t indx = GetColumnByName(columnName);
  if (indx >= 0 ) indx = GetDimensions(indx);
  return indx;
}
//____________________________________________________________________________
St_Table::EColumnType St_tableDescriptor::GetColumnType(const Char_t *columnName) const 
{
  Int_t indx = GetColumnByName(columnName);
  if (indx >= 0 ) indx = GetColumnType(indx);
  return EColumnType(indx);
}
