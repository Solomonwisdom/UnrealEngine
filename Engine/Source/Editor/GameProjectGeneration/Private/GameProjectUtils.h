// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

class GameProjectUtils
{
public:
	/** Context information used when validating that source code is being placed in the correct place for a given module */
	struct FModuleContextInfo
	{
		/** Path to the Source folder of the module */
		FString ModuleSourcePath;

		/** Name of the module */
		FString ModuleName;
	};

	/** Where is this class located within the Source folder? */
	enum class EClassLocation : uint8
	{
		/** The class is going to a user defined location (outside of the Public, Private, or Classes) folder for this module */
		UserDefined,

		/** The class is going to the Public folder for this module */
		Public,

		/** The class is going to the Private folder for this module */
		Private,

		/** The class is going to the Classes folder for this module */
		Classes,
	};

	/** Information used when creating a new class via AddCodeToProject */
	struct FNewClassInfo
	{
		/** The type of class we want to create */
		enum class EClassType : uint8
		{
			/** The new class is using a UObject as a base, consult BaseClass for the type */
			UObject,

			/** The new class should be an empty standard C++ class */
			EmptyCpp,

			/** The new class should be a Slate widget, deriving from SCompoundWidget */
			SlateWidget,

			/** The new class should be a Slate widget style, deriving from FSlateWidgetStyle, along with its associated UObject wrapper class */
			SlateWidgetStyle,
		};

		/** Default constructor; must produce an object which fails the IsSet check */
		FNewClassInfo()
			: ClassType(EClassType::UObject)
			, BaseClass(nullptr)
		{
		}

		/** Convenience constructor so you can construct from a EClassType */
		explicit FNewClassInfo(const EClassType InClassType)
			: ClassType(InClassType)
			, BaseClass(nullptr)
		{
		}

		/** Convenience constructor so you can construct from a UClass */
		explicit FNewClassInfo(const UClass* InBaseClass)
			: ClassType(EClassType::UObject)
			, BaseClass(InBaseClass)
		{
		}

		/** Check to see if this struct is set to something that could be used to create a new class */
		bool IsSet() const
		{
			return ClassType != EClassType::UObject || BaseClass;
		}

		/** Get the "friendly" class name to use in the UI */
		FString GetClassName() const;

		/** Get the class description to use in the UI */
		FString GetClassDescription() const;

		/** Get the class icon to use in the UI */
		const FSlateBrush* GetClassIcon() const;

		/** Get the C++ prefix used for this class type */
		FString GetClassPrefixCPP() const;

		/** Get the C++ class name; this may or may not be prefixed, but will always produce a valid C++ name via GetClassPrefix() + GetClassName() */
		FString GetClassNameCPP() const;

		/** Some classes may apply a particular suffix; this function returns the class name with those suffixes removed */
		FString GetCleanClassName(const FString& ClassName) const;

		/** Some classes may apply a particular suffix; this function returns the class name that will ultimately be used should that happen */
		FString GetFinalClassName(const FString& ClassName) const;

		/** Get the path needed to include this class into another file */
		bool GetIncludePath(FString& OutIncludePath) const;

		/** Given a class name, generate the header file (.h) that should be used for this class */
		FString GetHeaderFilename(const FString& ClassName) const;

		/** Given a class name, generate the source file (.cpp) that should be used for this class */
		FString GetSourceFilename(const FString& ClassName) const;

		/** Get the generation template filename to used based on the current class type */
		FString GetHeaderTemplateFilename() const;

		/** Get the generation template filename to used based on the current class type */
		FString GetSourceTemplateFilename() const;

		/** The type of class we want to create */
		EClassType ClassType;

		/** Base class information; if the ClassType is UObject */
		const UClass* BaseClass;
	};

	/** Returns true if the project filename is properly formed and does not conflict with another project */
	static bool IsValidProjectFileForCreation(const FString& ProjectFile, FText& OutFailReason);

	/** Opens the specified project, if it exists. Returns true if the project file is valid. On failure, OutFailReason will be populated. */
	static bool OpenProject(const FString& ProjectFile, FText& OutFailReason);

	/** Opens the code editing IDE for the specified project, if it exists. Returns true if the IDE could be opened. On failure, OutFailReason will be populated. */
	static bool OpenCodeIDE(const FString& ProjectFile, FText& OutFailReason);

	/** Creates the specified project file and all required folders. If TemplateFile is non-empty, it will be used as the template for creation. On failure, OutFailReason will be populated. */
	static bool CreateProject(const FString& NewProjectFile, const FString& TemplateFile, bool bShouldGenerateCode, bool bCopyStarterContent, FText& OutFailReason);

	/** Builds the binaries for a new project. */
	static bool BuildGameBinaries(const FString& ProjectFilename, FText& OutFailReason);

	/** Prompts the user to update his project file, if necessary. */
	static void CheckForOutOfDateGameProjectFile();

	/** Warn the user if the project filename is invalid in case they renamed it outside the editor */
	static void CheckAndWarnProjectFilenameValid();

	/** Updates the currently loaded project. Returns true if the project was updated successfully or if no update was needed */
	static bool UpdateGameProject(const FString &EngineIdentifier);

	/** Opens a dialog to add code files to a project */
	static void OpenAddCodeToProjectDialog();

	/** Returns true if the specified class name is properly formed and does not conflict with another class */
	static bool IsValidClassNameForCreation(const FString& NewClassName, FText& OutFailReason);

	/** Adds new source code to the project. When returning true, OutSyncFileAndLineNumber will be the the preferred target file to sync in the users code editing IDE, formatted for use with GenericApplication::GotoLineInSource */
	static bool AddCodeToProject(const FString& NewClassName, const FString& NewClassPath, const FNewClassInfo ParentClassInfo, FString& OutHeaderFilePath, FString& OutCppFilePath, FText& OutFailReason);

	/** Loads a template project definitions object from the TemplateDefs.ini file in the specified project */
	static UTemplateProjectDefs* LoadTemplateDefs(const FString& ProjectDirectory);

	/** Returns number of code files in the currently loaded project */
	static int32 GetProjectCodeFileCount();

	/** Returns the uproject template filename for the default project template. */
	static FString GetDefaultProjectTemplateFilename();

	/** Creates code project files for a new game project. On failure, OutFailReason will be populated. */
	static bool GenerateCodeProjectFiles(const FString& ProjectFilename, FText& OutFailReason);

	/** Generates a set of resource files for a game module */
	static bool GenerateGameResourceFiles(const FString& NewResourceFolderName, const FString& GameName, TArray<FString>& OutCreatedFiles, FText& OutFailReason);

	/** Returns true if there are starter content files available for instancing into new projects. */
	static bool IsStarterContentAvailableForNewProjects();

	/**
	 * Get the default module context information based on the current value of FApp
	 */
	static FModuleContextInfo GetCurrentModuleContextInfo();

	/** 
	 * Get the absolute root path under which all project source code must exist
	 *
	 * @param	bIncludeModuleName	Whether to include the module name in the root path?
	 * @param	ModuleInfo			Information about the module being validated; use GetCurrentModuleContextInfo() if you want to validate the currently loaded project
	 * 
	 * @return	The root path. Will always be an absolute path ending with a /
	 */
	static FString GetSourceRootPath(const bool bIncludeModuleName, const FModuleContextInfo& ModuleInfo);

	/** 
	 * Check to see if the given path is a valid place to put source code for this project (exists within the source root path) 
	 *
	 * @param	InPath				The path to check
	 * @param	bIncludeModuleName	Whether to require the module name in the root path? (is really used as a prefix so will allow MyModule, MyModuleEditor, etc)
	 * @param	ModuleInfo			Information about the module being validated; use GetCurrentModuleContextInfo() if you want to validate the currently loaded project
	 * @param	OutFailReason		Optional parameter to fill with failure information
	 * 
	 * @return	true if the path is valid, false otherwise
	 */
	static bool IsValidSourcePath(const FString& InPath, const bool bIncludeModuleName, const FModuleContextInfo& ModuleInfo, FText* const OutFailReason = nullptr);

	/** 
	 * Given the path provided, work out where generated .h and .cpp files would be placed
	 *
	 * @param	InPath				The path to use a base
	 * @param	OutModuleName		The module name extracted from the path (the part after GetSourceRootPath(false))
	 * @param	OutHeaderPath		The path where the .h file should be placed
	 * @param	OutSourcePath		The path where the .cpp file should be placed
	 * @param	ModuleInfo			Information about the module being validated; use GetCurrentModuleContextInfo() if you want to validate the currently loaded project
	 * @param	OutFailReason		Optional parameter to fill with failure information
	 * 
	 * @return	false if the paths are invalid
	 */
	static bool CalculateSourcePaths(const FString& InPath, FString& OutModuleName, FString& OutHeaderPath, FString& OutSourcePath, const FModuleContextInfo& ModuleInfo, FText* const OutFailReason = nullptr);

	/** 
	 * Given the path provided, work out where it's located within the Source folder
	 *
	 * @param	InPath				The path to use a base
	 * @param	OutModuleName		The module name extracted from the path (the part after GetSourceRootPath(false))
	 * @param	OutClassLocation	The location within the Source folder
	 * @param	ModuleInfo			Information about the module being validated; use GetCurrentModuleContextInfo() if you want to validate the currently loaded project
	 * @param	OutFailReason		Optional parameter to fill with failure information
	 * 
	 * @return	false if the paths are invalid
	 */
	static bool GetClassLocation(const FString& InPath, FString& OutModuleName, EClassLocation& OutClassLocation, const FModuleContextInfo& ModuleInfo, FText* const OutFailReason = nullptr);

	/** Creates a copy of a project directory in order to upgrade it. */
	static bool DuplicateProjectForUpgrade( const FString& InProjectFile, FString &OutNewProjectFile );

	/**
	 * Update the list of supported target platforms based upon the parameters provided
	 * This will take care of checking out and saving the updated .uproject file automatically
	 * 
	 * @param	InPlatformName		Name of the platform to target (eg, WindowsNoEditor)
	 * @param	bIsSupported		true if the platform should be supported by this project, false if it should not
	 */
	static void UpdateSupportedTargetPlatforms(const FName& InPlatformName, const bool bIsSupported);

	/** Clear the list of supported target platforms */
	static void ClearSupportedTargetPlatforms();

private:
	/** Generates a new project without using a template project */
	static bool GenerateProjectFromScratch(const FString& NewProjectFile, bool bShouldGenerateCode, bool bCopyStarterContent, FText& OutFailReason);

	/** Generates a new project using a template project */
	static bool CreateProjectFromTemplate(const FString& NewProjectFile, const FString& TemplateFile, bool bShouldGenerateCode, bool bCopyStarterContent, FText& OutFailReason);

	/** Copies starter content into the specified project folder. */
	static bool CopyStarterContent(const FString& DestProjectFolder, FText& OutFailReason);

	/** Returns list of starter content files */
	static void GetStarterContentFiles(TArray<FString>& OutFilenames);

	/** Returns the template defs ini filename */
	static FString GetTemplateDefsFilename();

	/** Checks the name for illegal characters */
	static bool NameContainsOnlyLegalCharacters(const FString& TestName, FString& OutIllegalCharacters);

	/** Checks the name for an underscore and the existence of XB1 XDK */
	static bool NameContainsUnderscoreAndXB1Installed(const FString& TestName);

	/** Returns true if the project file exists on disk */
	static bool ProjectFileExists(const FString& ProjectFile);

	/** Returns true if any project files exist in the given folder */
	static bool AnyProjectFilesExistInFolder(const FString& Path);

	/** Returns true if file cleanup on failure is enabled, false if not */
	static bool CleanupIsEnabled();

	/** Deletes the specified list of files that were created during file creation */
	static void DeleteCreatedFiles(const FString& RootFolder, const TArray<FString>& CreatedFiles);

	/** Deletes any files that were generated by the generate project files step */
	static void DeleteGeneratedProjectFiles(const FString& NewProjectFile);

	/** Deletes any files that were generated by the build step */
	static void DeleteGeneratedBuildFiles(const FString& NewProjectFolder);

	/** Creates ini files for a new project. On failure, OutFailReason will be populated. */
	static bool GenerateConfigFiles(const FString& NewProjectPath, const FString& NewProjectName, bool bShouldGenerateCode, bool bCopyStarterContent, TArray<FString>& OutCreatedFiles, FText& OutFailReason);

	/** Creates the basic source code for a new project. On failure, OutFailReason will be populated. */
	static bool GenerateBasicSourceCode(const FString& NewProjectSourcePath, const FString& NewProjectName, TArray<FString>& OutGeneratedStartupModuleNames, TArray<FString>& OutCreatedFiles, FText& OutFailReason);

	/** Creates the game framework source code for a new project (Pawn, GameMode, PlayerControlleR). On failure, OutFailReason will be populated. */
	static bool GenerateGameFrameworkSourceCode(const FString& NewProjectSourcePath, const FString& NewProjectName, TArray<FString>& OutCreatedFiles, FText& OutFailReason);

	/** Creates the batch file to regenerate code projects. */
	static bool GenerateCodeProjectGenerationBatchFile(const FString& ProjectFolder, TArray<FString>& OutCreatedFiles, FText& OutFailReason);

	/** Creates the batch file for launching the editor or game */
	static bool GenerateLaunchBatchFile(const FString& ProjectName, const FString& ProjectFolder, bool bLaunchEditor, TArray<FString>& OutCreatedFiles, FText& OutFailReason);


	/** Returns the contents of the specified template file */
	static bool ReadTemplateFile(const FString& TemplateFileName, FString& OutFileContents, FText& OutFailReason);

	/** Writes an output file. OutputFilename includes a path */
	static bool WriteOutputFile(const FString& OutputFilename, const FString& OutputFileContents, FText& OutFailReason);

	/** Returns the copyright line used at the top of all files */
	static FString MakeCopyrightLine();

	/** Returns a comma delimited string comprised of all the elements in InList. If bPlaceQuotesAroundEveryElement, every element is within quotes. */
	static FString MakeCommaDelimitedList(const TArray<FString>& InList, bool bPlaceQuotesAroundEveryElement = true);

	/** Returns a list of #include lines formed from InList */
	static FString MakeIncludeList(const TArray<FString>& InList);

	/** Generates a header file for a UObject class. OutSyncLocation is a string representing the preferred cursor sync location for this file after creation. */
	static bool GenerateClassHeaderFile(const FString& NewHeaderFileName, const FString UnPrefixedClassName, const FNewClassInfo ParentClassInfo, const TArray<FString>& ClassSpecifierList, const FString& ClassProperties, const FString& ClassFunctionDeclarations, FString& OutSyncLocation, const FModuleContextInfo& ModuleInfo, FText& OutFailReason);

	/** Generates a cpp file for a UObject class */
	static bool GenerateClassCPPFile(const FString& NewCPPFileName, const FString UnPrefixedClassName, const FNewClassInfo ParentClassInfo, const TArray<FString>& AdditionalIncludes, const TArray<FString>& PropertyOverrides, const FString& AdditionalMemberDefinitions, const FModuleContextInfo& ModuleInfo, FText& OutFailReason);

	/** Generates a Build.cs file for a game module */
	static bool GenerateGameModuleBuildFile(const FString& NewBuildFileName, const FString& ModuleName, const TArray<FString>& PublicDependencyModuleNames, const TArray<FString>& PrivateDependencyModuleNames, FText& OutFailReason);

	/** Generates a Target.cs file for a game module */
	static bool GenerateGameModuleTargetFile(const FString& NewTargetFileName, const FString& ModuleName, const TArray<FString>& ExtraModuleNames, FText& OutFailReason);

	/** Generates a resource file for a game module */
	static bool GenerateGameResourceFile(const FString& NewResourceFolderName, const FString& TemplateFilename, const FString& GameName, TArray<FString>& OutCreatedFiles, FText& OutFailReason);

	/** Generates a Build.cs file for a Editor module */
	static bool GenerateEditorModuleBuildFile(const FString& NewBuildFileName, const FString& ModuleName, const TArray<FString>& PublicDependencyModuleNames, const TArray<FString>& PrivateDependencyModuleNames, FText& OutFailReason);

	/** Generates a Target.cs file for a Editor module */
	static bool GenerateEditorModuleTargetFile(const FString& NewTargetFileName, const FString& ModuleName, const TArray<FString>& ExtraModuleNames, FText& OutFailReason);

	/** Generates a main game module cpp file */
	static bool GenerateGameModuleCPPFile(const FString& NewGameModuleCPPFileName, const FString& ModuleName, const FString& GameName, FText& OutFailReason);

	/** Generates a main game module header file */
	static bool GenerateGameModuleHeaderFile(const FString& NewGameModuleHeaderFileName, const TArray<FString>& PublicHeaderIncludes, FText& OutFailReason);

	/** Handler for when the user confirms a project update */
	static void OnUpdateProjectConfirm();

	/**
	 * Updates the projects, and optionally the modules names
	 *
	 * @param StartupModuleNames	if specified, replaces the existing module names with this version
	 */
	static void UpdateProject(const TArray<FString>* StartupModuleNames);

	/** Handler for when the user opts out of a project update */
	static void OnUpdateProjectCancel();

	/**
	 * Updates the loaded game project file to the current version
	 *
	 * @param ProjectFilename		The name of the project (used to checkout from source control)
	 * @param StartupModuleNames	if specified, replaces the existing module names with this version
	 * @param OutbWasCheckedOut		Out, whether or not the project was checked out from source control
	 * @param OutFailReason			Out, if unsuccessful this is the reason why

	 * @return true, if successful
	 */
	static bool UpdateGameProjectFile(const FString& ProjectFilename, const FString& EngineIdentifier, const TArray<FString>* StartupModuleNames, bool& OutbWasCheckedOut, FText& OutFailReason);

	/** Checks the specified game project file out from source control */
	static bool CheckoutGameProjectFile(const FString& ProjectFilename, FText& OutFailReason);

	/** Returns true if the currently loaded project has code files */
	static bool ProjectHasCodeFiles();

	/** Internal handler for AddCodeToProject*/
	static bool AddCodeToProject_Internal(const FString& NewClassName, const FString& NewClassPath, const FNewClassInfo ParentClassInfo, FString& OutHeaderFilePath, FString& OutCppFilePath, FText& OutFailReason);

	/** Handler for the user confirming they've read the name legnth warning */
	static void OnWarningReasonOk();

	static int32 CalculateSubfolderCount(const FString& Filepath, const FString& RelativeRoot);

private:
	static TWeakPtr<SNotificationItem> UpdateGameProjectNotification;
	static TWeakPtr<SNotificationItem> WarningProjectNameNotification;
};
