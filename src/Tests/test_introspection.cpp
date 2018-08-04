/*
-----------------------------------
platform layer:		3146
	renderer		1036

App layer:			6017
	game:			3107
	
common lib			3033
-----------------------------------
Total 15,325
-----------------------------------
*/
struct IntrospectVarLabel
{
	char label[32];
	i32 labelLength = 32;
}

enum ZDataType
{
	Int32,
	Float32,
	Bool,
	Vector3D,
	Normal3D,
	String
};

struct IntrospectVar
{
	// order in containing structure
	i32 indexInStruct;
	// offset from start of structure to start of this value
	i32 offset;
	// type describes how to read/write this value
	ZDataType type;
	// don't want to store the label in this
	i32 labelId;
	
};
