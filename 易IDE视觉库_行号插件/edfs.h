#pragma once

#include "efs.h"

typedef HINSTANCE EADDRESS;
typedef HINSTANCE EDATA;
typedef UINT eVariableAttr;
typedef UINT eConstAttr;
typedef UINT eConstType;
typedef UINT eTAG;

struct eVariable
{
	DataType Type;
	eVariableAttr Attribute;
	UINT Dimension;
	EADDRESS DimensionMember;
	PCSTR Name;
	PCSTR Remark;
};

struct eResource
{
	eConstAttr Attribute;
	PCSTR Name;
	PCSTR Remark;
	eConstType Type;
	UINT Length;
	EADDRESS Data;
};

struct eArray
{
	EADDRESS* _vptr;
	EADDRESS Address;
	UINT Count;
	UINT MaxCount;
};

struct eStream
{
	EADDRESS* _vptr;
	EADDRESS pBuffer;
	EADDRESS Address;
	UINT MaxLength;
	UINT Length;
};

struct eList
{
	EADDRESS* _vptr;
	UINT Count;
	eStream List;
	EDATA pData;
};

struct eBase
{
	PCSTR Name;
	PCSTR Remark;
	ETagStatus Status;
};

struct eStruct : eBase
{
	eList Member;
	EDATA pData;
};

struct eMethod
{
	PCSTR Name;
	PCSTR EPack;
	PCSTR Remark;
	ETAG Assembly;
	EMethodAttr Status;
	DataType ReturnType;
	eList Variable;
	eList Parameter;
	eStream RowOffset;
	eStream FlowOffset;
	eStream MethodOffset;
	eStream VarOffset;
	eStream ParamOffset;
	eStream Code;
	EDATA pData;
};

struct eAssembly : eBase
{
	eList Variable;
	eStream Method;
	LPCVOID unk1;
	ETAG Base;
	LPCVOID unk2[2];
	EDATA pData;
};

struct eDll : eBase
{
	DataType ReturnType;
	eList Member;
	PCSTR Lib;
	PCSTR Command;
	LPCVOID unk[2];
	EDATA pData;
};

struct eECInfo
{
	LPCVOID unk;
	PCSTR Name;
	PCSTR Path;
	LPCVOID unk2[13];
};

struct eLibInfo
{
	eArray Names;
	UINT Increment;
	eArray Infos;
};