// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/VRPathFollowingComponent.h"
PRAGMA_DISABLE_OPTIMIZATION
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeVRPathFollowingComponent() {}
// Cross Module References
	VREXPANSIONPLUGIN_API UClass* Z_Construct_UClass_UVRPathFollowingComponent_NoRegister();
	VREXPANSIONPLUGIN_API UClass* Z_Construct_UClass_UVRPathFollowingComponent();
	AIMODULE_API UClass* Z_Construct_UClass_UPathFollowingComponent();
	UPackage* Z_Construct_UPackage__Script_VRExpansionPlugin();
	VREXPANSIONPLUGIN_API UClass* Z_Construct_UClass_UVRCharacterMovementComponent_NoRegister();
// End Cross Module References
	void UVRPathFollowingComponent::StaticRegisterNativesUVRPathFollowingComponent()
	{
	}
	UClass* Z_Construct_UClass_UVRPathFollowingComponent_NoRegister()
	{
		return UVRPathFollowingComponent::StaticClass();
	}
	UClass* Z_Construct_UClass_UVRPathFollowingComponent()
	{
		static UClass* OuterClass = NULL;
		if (!OuterClass)
		{
			Z_Construct_UClass_UPathFollowingComponent();
			Z_Construct_UPackage__Script_VRExpansionPlugin();
			OuterClass = UVRPathFollowingComponent::StaticClass();
			if (!(OuterClass->ClassFlags & CLASS_Constructed))
			{
				UObjectForceRegistration(OuterClass);
				OuterClass->ClassFlags |= (EClassFlags)0x20B00080u;


				UProperty* NewProp_VRMovementComp = new(EC_InternalUseOnlyConstructor, OuterClass, TEXT("VRMovementComp"), RF_Public|RF_Transient|RF_MarkAsNative) UObjectProperty(CPP_PROPERTY_BASE(VRMovementComp, UVRPathFollowingComponent), 0x0010000000082008, Z_Construct_UClass_UVRCharacterMovementComponent_NoRegister());
				static TCppClassTypeInfo<TCppClassTypeTraits<UVRPathFollowingComponent> > StaticCppClassTypeInfo;
				OuterClass->SetCppTypeInfo(&StaticCppClassTypeInfo);
				OuterClass->StaticLink();
#if WITH_METADATA
				UMetaData* MetaData = OuterClass->GetOutermost()->GetMetaData();
				MetaData->SetValue(OuterClass, TEXT("IncludePath"), TEXT("VRPathFollowingComponent.h"));
				MetaData->SetValue(OuterClass, TEXT("ModuleRelativePath"), TEXT("Public/VRPathFollowingComponent.h"));
				MetaData->SetValue(NewProp_VRMovementComp, TEXT("EditInline"), TEXT("true"));
				MetaData->SetValue(NewProp_VRMovementComp, TEXT("ModuleRelativePath"), TEXT("Public/VRPathFollowingComponent.h"));
#endif
			}
		}
		check(OuterClass->GetClass());
		return OuterClass;
	}
	IMPLEMENT_CLASS(UVRPathFollowingComponent, 3382906392);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UVRPathFollowingComponent(Z_Construct_UClass_UVRPathFollowingComponent, &UVRPathFollowingComponent::StaticClass, TEXT("/Script/VRExpansionPlugin"), TEXT("UVRPathFollowingComponent"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UVRPathFollowingComponent);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
PRAGMA_ENABLE_OPTIMIZATION
