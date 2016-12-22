##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=testapp
ConfigurationName      :=Debug
WorkspacePath          :=C:/Users/al302700/code/Jasper2
ProjectPath            :=C:/Users/al302700/code/Jasper2/testapp
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=AL302700
Date                   :=13/12/2016
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :=link.exe  /nologo
SharedObjectLinkerName :=link.exe /DLL  /nologo
ObjectSuffix           :=.obj
DependSuffix           :=
PreprocessSuffix       :=
DebugSwitch            :=/Zi 
IncludeSwitch          :=/I
LibrarySwitch          := 
OutputSwitch           :=/OUT:
LibraryPathSwitch      :=/LIBPATH:
PreprocessorSwitch     :=/D
SourceSwitch           :=
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=/Fo
ArchiveOutputSwitch    :=/OUT:
PreprocessOnlySwitch   :=
ObjectsFileList        :="testapp.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  -lGLEW -lSDL2 -lGL -lBulletDynamics -lBulletCollision -lLinearMath -lassimp
IncludePath            := $(IncludeSwitch)""C:/Program Files/Microsoft Visual Studio 8/VC/include"" $(IncludeSwitch).  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./include/ $(IncludeSwitch)./include/jasper/ $(IncludeSwitch)/usr/include/bullet/ 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                :=$(LibraryPathSwitch)""C:/Program Files/Microsoft Visual Studio 8/VC/lib""  $(LibraryPathSwitch). $(LibraryPathSwitch)/usr/local/lib/ 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := lib.exe  /nologo
CXX      := cl.exe /nologo /c
CC       := cl.exe /nologo /c
CXXFLAGS :=  -Wmain -g -O0 -std=c++14 -Wall $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := NASM


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/Camera.cpp$(ObjectSuffix) $(IntermediateDirectory)/CapsuleCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/CharacterController.cpp$(ObjectSuffix) $(IntermediateDirectory)/Component.cpp$(ObjectSuffix) $(IntermediateDirectory)/ConvexHullCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/Cube.cpp$(ObjectSuffix) $(IntermediateDirectory)/CylinderCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/FontRenderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/FontShader.cpp$(ObjectSuffix) $(IntermediateDirectory)/GBuffer.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/GLBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/GLWindow.cpp$(ObjectSuffix) $(IntermediateDirectory)/Lights.cpp$(ObjectSuffix) $(IntermediateDirectory)/LitShader.cpp$(ObjectSuffix) $(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Material.cpp$(ObjectSuffix) $(IntermediateDirectory)/matrix.cpp$(ObjectSuffix) $(IntermediateDirectory)/Memory.cpp$(ObjectSuffix) $(IntermediateDirectory)/MeshRenderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Model.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/PhysicsCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/PhysicsDebugDraw.cpp$(ObjectSuffix) $(IntermediateDirectory)/PhysicsWorld.cpp$(ObjectSuffix) $(IntermediateDirectory)/PlaneCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/Quad.cpp$(ObjectSuffix) $(IntermediateDirectory)/quaternion.cpp$(ObjectSuffix) $(IntermediateDirectory)/Renderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Room.cpp$(ObjectSuffix) $(IntermediateDirectory)/Scene.cpp$(ObjectSuffix) $(IntermediateDirectory)/Shader.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/ShadowMapShader.cpp$(ObjectSuffix) $(IntermediateDirectory)/SkyboxRenderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/SkyboxShader.cpp$(ObjectSuffix) $(IntermediateDirectory)/Sphere.cpp$(ObjectSuffix) $(IntermediateDirectory)/SphereCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/Texture.cpp$(ObjectSuffix) $(IntermediateDirectory)/Transform.cpp$(ObjectSuffix) $(IntermediateDirectory)/Triangle.cpp$(ObjectSuffix) $(IntermediateDirectory)/DefaultScript.cpp$(ObjectSuffix) $(IntermediateDirectory)/BoxCollider.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/include_jasper_CapsuleCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) $(IntermediateDirectory)/LauncherScript.cpp$(ObjectSuffix) $(IntermediateDirectory)/RotateInPlaceScript.cpp$(ObjectSuffix) $(IntermediateDirectory)/Mesh.cpp$(ObjectSuffix) $(IntermediateDirectory)/RotateAboutPointScript.cpp$(ObjectSuffix) $(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(ObjectSuffix) $(IntermediateDirectory)/BasicShader.cpp$(ObjectSuffix) $(IntermediateDirectory)/imgui.cpp$(ObjectSuffix) $(IntermediateDirectory)/imgui_draw.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/GuiRenderer.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Camera.cpp$(ObjectSuffix): Camera.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Camera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Camera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CapsuleCollider.cpp$(ObjectSuffix): CapsuleCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/CapsuleCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CapsuleCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CharacterController.cpp$(ObjectSuffix): CharacterController.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/CharacterController.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CharacterController.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Component.cpp$(ObjectSuffix): Component.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Component.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Component.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ConvexHullCollider.cpp$(ObjectSuffix): ConvexHullCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/ConvexHullCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ConvexHullCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Cube.cpp$(ObjectSuffix): Cube.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Cube.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Cube.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CylinderCollider.cpp$(ObjectSuffix): CylinderCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/CylinderCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CylinderCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FontRenderer.cpp$(ObjectSuffix): FontRenderer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/FontRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FontRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FontShader.cpp$(ObjectSuffix): FontShader.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/FontShader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FontShader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GBuffer.cpp$(ObjectSuffix): GBuffer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/GBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GLBuffer.cpp$(ObjectSuffix): GLBuffer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/GLBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GLBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GLWindow.cpp$(ObjectSuffix): GLWindow.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/GLWindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GLWindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Lights.cpp$(ObjectSuffix): Lights.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Lights.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Lights.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LitShader.cpp$(ObjectSuffix): LitShader.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/LitShader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LitShader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Material.cpp$(ObjectSuffix): Material.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Material.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Material.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/matrix.cpp$(ObjectSuffix): matrix.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/matrix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/matrix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Memory.cpp$(ObjectSuffix): Memory.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Memory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Memory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MeshRenderer.cpp$(ObjectSuffix): MeshRenderer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/MeshRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MeshRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Model.cpp$(ObjectSuffix): Model.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Model.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Model.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PhysicsCollider.cpp$(ObjectSuffix): PhysicsCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/PhysicsCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PhysicsCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(ObjectSuffix): PhysicsDebugDraw.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/PhysicsDebugDraw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PhysicsWorld.cpp$(ObjectSuffix): PhysicsWorld.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/PhysicsWorld.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PhysicsWorld.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PlaneCollider.cpp$(ObjectSuffix): PlaneCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/PlaneCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PlaneCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Quad.cpp$(ObjectSuffix): Quad.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Quad.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Quad.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/quaternion.cpp$(ObjectSuffix): quaternion.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/quaternion.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/quaternion.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Renderer.cpp$(ObjectSuffix): Renderer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Room.cpp$(ObjectSuffix): Room.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Room.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Room.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Scene.cpp$(ObjectSuffix): Scene.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Scene.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Scene.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Shader.cpp$(ObjectSuffix): Shader.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Shader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Shader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ShadowMapShader.cpp$(ObjectSuffix): ShadowMapShader.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/ShadowMapShader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ShadowMapShader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SkyboxRenderer.cpp$(ObjectSuffix): SkyboxRenderer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/SkyboxRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SkyboxRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SkyboxShader.cpp$(ObjectSuffix): SkyboxShader.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/SkyboxShader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SkyboxShader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Sphere.cpp$(ObjectSuffix): Sphere.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Sphere.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Sphere.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SphereCollider.cpp$(ObjectSuffix): SphereCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/SphereCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SphereCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Texture.cpp$(ObjectSuffix): Texture.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Texture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Texture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Transform.cpp$(ObjectSuffix): Transform.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Transform.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Transform.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Triangle.cpp$(ObjectSuffix): Triangle.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Triangle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Triangle.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/DefaultScript.cpp$(ObjectSuffix): DefaultScript.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/DefaultScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/DefaultScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BoxCollider.cpp$(ObjectSuffix): BoxCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/BoxCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BoxCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/include_jasper_CapsuleCollider.cpp$(ObjectSuffix): include/jasper/CapsuleCollider.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/include/jasper/CapsuleCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/include_jasper_CapsuleCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix): GameObject.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/GameObject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LauncherScript.cpp$(ObjectSuffix): LauncherScript.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/LauncherScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LauncherScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/RotateInPlaceScript.cpp$(ObjectSuffix): RotateInPlaceScript.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/RotateInPlaceScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/RotateInPlaceScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Mesh.cpp$(ObjectSuffix): Mesh.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/Mesh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Mesh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/RotateAboutPointScript.cpp$(ObjectSuffix): RotateAboutPointScript.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/RotateAboutPointScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/RotateAboutPointScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(ObjectSuffix): include/jasper/AssetSerializer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/include/jasper/AssetSerializer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/BasicShader.cpp$(ObjectSuffix): BasicShader.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/BasicShader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/BasicShader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/imgui.cpp$(ObjectSuffix): imgui.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/imgui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/imgui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/imgui_draw.cpp$(ObjectSuffix): imgui_draw.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/imgui_draw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/imgui_draw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GuiRenderer.cpp$(ObjectSuffix): GuiRenderer.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/Users/al302700/code/Jasper2/testapp/GuiRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GuiRenderer.cpp$(ObjectSuffix) $(IncludePath)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


