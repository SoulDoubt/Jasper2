##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=testapp
ConfigurationName      :=Debug
WorkspacePath          :=/home/al/code/testapp
ProjectPath            :=/home/al/code/testapp/testapp
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Al
Date                   :=06/02/17
CodeLitePath           :=/home/al/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="testapp.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lGLEW -lSDL2 -lGL -lBulletDynamics -lBulletCollision -lLinearMath -lassimp -lvhacd -pthread -lgomp  -pg
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./include/ $(IncludeSwitch)./include/jasper/ $(IncludeSwitch)/usr/include/bullet/ 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/usr/local/lib/ 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -Wmain -g  -pg  -O0 -std=c++14 -Wall $(Preprocessors)
CFLAGS   :=  -g -pg  -O0 -std=c++14 -Wall -pthread -fopenmp $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/Camera.cpp$(ObjectSuffix) $(IntermediateDirectory)/CharacterController.cpp$(ObjectSuffix) $(IntermediateDirectory)/Component.cpp$(ObjectSuffix) $(IntermediateDirectory)/ConvexHullCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/Cube.cpp$(ObjectSuffix) $(IntermediateDirectory)/FontRenderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/GBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/GLBuffer.cpp$(ObjectSuffix) $(IntermediateDirectory)/GLWindow.cpp$(ObjectSuffix) $(IntermediateDirectory)/Lights.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/Material.cpp$(ObjectSuffix) $(IntermediateDirectory)/matrix.cpp$(ObjectSuffix) $(IntermediateDirectory)/Memory.cpp$(ObjectSuffix) $(IntermediateDirectory)/MeshRenderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Model.cpp$(ObjectSuffix) $(IntermediateDirectory)/PhysicsCollider.cpp$(ObjectSuffix) $(IntermediateDirectory)/PhysicsDebugDraw.cpp$(ObjectSuffix) $(IntermediateDirectory)/PhysicsWorld.cpp$(ObjectSuffix) $(IntermediateDirectory)/Quad.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/quaternion.cpp$(ObjectSuffix) $(IntermediateDirectory)/Renderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Scene.cpp$(ObjectSuffix) $(IntermediateDirectory)/Shader.cpp$(ObjectSuffix) $(IntermediateDirectory)/SkyboxRenderer.cpp$(ObjectSuffix) $(IntermediateDirectory)/Sphere.cpp$(ObjectSuffix) $(IntermediateDirectory)/Texture.cpp$(ObjectSuffix) $(IntermediateDirectory)/Transform.cpp$(ObjectSuffix) $(IntermediateDirectory)/Triangle.cpp$(ObjectSuffix) $(IntermediateDirectory)/DefaultScript.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) $(IntermediateDirectory)/LauncherScript.cpp$(ObjectSuffix) $(IntermediateDirectory)/RotateInPlaceScript.cpp$(ObjectSuffix) $(IntermediateDirectory)/Mesh.cpp$(ObjectSuffix) $(IntermediateDirectory)/RotateAboutPointScript.cpp$(ObjectSuffix) $(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(ObjectSuffix) $(IntermediateDirectory)/imgui.cpp$(ObjectSuffix) $(IntermediateDirectory)/imgui_draw.cpp$(ObjectSuffix) $(IntermediateDirectory)/Scriptable.cpp$(ObjectSuffix) $(IntermediateDirectory)/ParticleSystem.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(ObjectSuffix) 



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
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/Camera.cpp$(ObjectSuffix): Camera.cpp $(IntermediateDirectory)/Camera.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Camera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Camera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Camera.cpp$(DependSuffix): Camera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Camera.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Camera.cpp$(DependSuffix) -MM Camera.cpp

$(IntermediateDirectory)/Camera.cpp$(PreprocessSuffix): Camera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Camera.cpp$(PreprocessSuffix) Camera.cpp

$(IntermediateDirectory)/CharacterController.cpp$(ObjectSuffix): CharacterController.cpp $(IntermediateDirectory)/CharacterController.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/CharacterController.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CharacterController.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CharacterController.cpp$(DependSuffix): CharacterController.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/CharacterController.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/CharacterController.cpp$(DependSuffix) -MM CharacterController.cpp

$(IntermediateDirectory)/CharacterController.cpp$(PreprocessSuffix): CharacterController.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CharacterController.cpp$(PreprocessSuffix) CharacterController.cpp

$(IntermediateDirectory)/Component.cpp$(ObjectSuffix): Component.cpp $(IntermediateDirectory)/Component.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Component.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Component.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Component.cpp$(DependSuffix): Component.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Component.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Component.cpp$(DependSuffix) -MM Component.cpp

$(IntermediateDirectory)/Component.cpp$(PreprocessSuffix): Component.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Component.cpp$(PreprocessSuffix) Component.cpp

$(IntermediateDirectory)/ConvexHullCollider.cpp$(ObjectSuffix): ConvexHullCollider.cpp $(IntermediateDirectory)/ConvexHullCollider.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/ConvexHullCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ConvexHullCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ConvexHullCollider.cpp$(DependSuffix): ConvexHullCollider.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ConvexHullCollider.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ConvexHullCollider.cpp$(DependSuffix) -MM ConvexHullCollider.cpp

$(IntermediateDirectory)/ConvexHullCollider.cpp$(PreprocessSuffix): ConvexHullCollider.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ConvexHullCollider.cpp$(PreprocessSuffix) ConvexHullCollider.cpp

$(IntermediateDirectory)/Cube.cpp$(ObjectSuffix): Cube.cpp $(IntermediateDirectory)/Cube.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Cube.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Cube.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Cube.cpp$(DependSuffix): Cube.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Cube.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Cube.cpp$(DependSuffix) -MM Cube.cpp

$(IntermediateDirectory)/Cube.cpp$(PreprocessSuffix): Cube.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Cube.cpp$(PreprocessSuffix) Cube.cpp

$(IntermediateDirectory)/FontRenderer.cpp$(ObjectSuffix): FontRenderer.cpp $(IntermediateDirectory)/FontRenderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/FontRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/FontRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/FontRenderer.cpp$(DependSuffix): FontRenderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/FontRenderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/FontRenderer.cpp$(DependSuffix) -MM FontRenderer.cpp

$(IntermediateDirectory)/FontRenderer.cpp$(PreprocessSuffix): FontRenderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/FontRenderer.cpp$(PreprocessSuffix) FontRenderer.cpp

$(IntermediateDirectory)/GBuffer.cpp$(ObjectSuffix): GBuffer.cpp $(IntermediateDirectory)/GBuffer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/GBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GBuffer.cpp$(DependSuffix): GBuffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GBuffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GBuffer.cpp$(DependSuffix) -MM GBuffer.cpp

$(IntermediateDirectory)/GBuffer.cpp$(PreprocessSuffix): GBuffer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GBuffer.cpp$(PreprocessSuffix) GBuffer.cpp

$(IntermediateDirectory)/GLBuffer.cpp$(ObjectSuffix): GLBuffer.cpp $(IntermediateDirectory)/GLBuffer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/GLBuffer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GLBuffer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GLBuffer.cpp$(DependSuffix): GLBuffer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GLBuffer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GLBuffer.cpp$(DependSuffix) -MM GLBuffer.cpp

$(IntermediateDirectory)/GLBuffer.cpp$(PreprocessSuffix): GLBuffer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GLBuffer.cpp$(PreprocessSuffix) GLBuffer.cpp

$(IntermediateDirectory)/GLWindow.cpp$(ObjectSuffix): GLWindow.cpp $(IntermediateDirectory)/GLWindow.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/GLWindow.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GLWindow.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GLWindow.cpp$(DependSuffix): GLWindow.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GLWindow.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GLWindow.cpp$(DependSuffix) -MM GLWindow.cpp

$(IntermediateDirectory)/GLWindow.cpp$(PreprocessSuffix): GLWindow.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GLWindow.cpp$(PreprocessSuffix) GLWindow.cpp

$(IntermediateDirectory)/Lights.cpp$(ObjectSuffix): Lights.cpp $(IntermediateDirectory)/Lights.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Lights.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Lights.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Lights.cpp$(DependSuffix): Lights.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Lights.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Lights.cpp$(DependSuffix) -MM Lights.cpp

$(IntermediateDirectory)/Lights.cpp$(PreprocessSuffix): Lights.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Lights.cpp$(PreprocessSuffix) Lights.cpp

$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/Material.cpp$(ObjectSuffix): Material.cpp $(IntermediateDirectory)/Material.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Material.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Material.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Material.cpp$(DependSuffix): Material.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Material.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Material.cpp$(DependSuffix) -MM Material.cpp

$(IntermediateDirectory)/Material.cpp$(PreprocessSuffix): Material.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Material.cpp$(PreprocessSuffix) Material.cpp

$(IntermediateDirectory)/matrix.cpp$(ObjectSuffix): matrix.cpp $(IntermediateDirectory)/matrix.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/matrix.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/matrix.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/matrix.cpp$(DependSuffix): matrix.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/matrix.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/matrix.cpp$(DependSuffix) -MM matrix.cpp

$(IntermediateDirectory)/matrix.cpp$(PreprocessSuffix): matrix.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/matrix.cpp$(PreprocessSuffix) matrix.cpp

$(IntermediateDirectory)/Memory.cpp$(ObjectSuffix): Memory.cpp $(IntermediateDirectory)/Memory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Memory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Memory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Memory.cpp$(DependSuffix): Memory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Memory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Memory.cpp$(DependSuffix) -MM Memory.cpp

$(IntermediateDirectory)/Memory.cpp$(PreprocessSuffix): Memory.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Memory.cpp$(PreprocessSuffix) Memory.cpp

$(IntermediateDirectory)/MeshRenderer.cpp$(ObjectSuffix): MeshRenderer.cpp $(IntermediateDirectory)/MeshRenderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/MeshRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/MeshRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/MeshRenderer.cpp$(DependSuffix): MeshRenderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/MeshRenderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/MeshRenderer.cpp$(DependSuffix) -MM MeshRenderer.cpp

$(IntermediateDirectory)/MeshRenderer.cpp$(PreprocessSuffix): MeshRenderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/MeshRenderer.cpp$(PreprocessSuffix) MeshRenderer.cpp

$(IntermediateDirectory)/Model.cpp$(ObjectSuffix): Model.cpp $(IntermediateDirectory)/Model.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Model.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Model.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Model.cpp$(DependSuffix): Model.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Model.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Model.cpp$(DependSuffix) -MM Model.cpp

$(IntermediateDirectory)/Model.cpp$(PreprocessSuffix): Model.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Model.cpp$(PreprocessSuffix) Model.cpp

$(IntermediateDirectory)/PhysicsCollider.cpp$(ObjectSuffix): PhysicsCollider.cpp $(IntermediateDirectory)/PhysicsCollider.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/PhysicsCollider.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PhysicsCollider.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PhysicsCollider.cpp$(DependSuffix): PhysicsCollider.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PhysicsCollider.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PhysicsCollider.cpp$(DependSuffix) -MM PhysicsCollider.cpp

$(IntermediateDirectory)/PhysicsCollider.cpp$(PreprocessSuffix): PhysicsCollider.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PhysicsCollider.cpp$(PreprocessSuffix) PhysicsCollider.cpp

$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(ObjectSuffix): PhysicsDebugDraw.cpp $(IntermediateDirectory)/PhysicsDebugDraw.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/PhysicsDebugDraw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(DependSuffix): PhysicsDebugDraw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(DependSuffix) -MM PhysicsDebugDraw.cpp

$(IntermediateDirectory)/PhysicsDebugDraw.cpp$(PreprocessSuffix): PhysicsDebugDraw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PhysicsDebugDraw.cpp$(PreprocessSuffix) PhysicsDebugDraw.cpp

$(IntermediateDirectory)/PhysicsWorld.cpp$(ObjectSuffix): PhysicsWorld.cpp $(IntermediateDirectory)/PhysicsWorld.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/PhysicsWorld.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/PhysicsWorld.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/PhysicsWorld.cpp$(DependSuffix): PhysicsWorld.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/PhysicsWorld.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/PhysicsWorld.cpp$(DependSuffix) -MM PhysicsWorld.cpp

$(IntermediateDirectory)/PhysicsWorld.cpp$(PreprocessSuffix): PhysicsWorld.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/PhysicsWorld.cpp$(PreprocessSuffix) PhysicsWorld.cpp

$(IntermediateDirectory)/Quad.cpp$(ObjectSuffix): Quad.cpp $(IntermediateDirectory)/Quad.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Quad.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Quad.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Quad.cpp$(DependSuffix): Quad.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Quad.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Quad.cpp$(DependSuffix) -MM Quad.cpp

$(IntermediateDirectory)/Quad.cpp$(PreprocessSuffix): Quad.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Quad.cpp$(PreprocessSuffix) Quad.cpp

$(IntermediateDirectory)/quaternion.cpp$(ObjectSuffix): quaternion.cpp $(IntermediateDirectory)/quaternion.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/quaternion.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/quaternion.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/quaternion.cpp$(DependSuffix): quaternion.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/quaternion.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/quaternion.cpp$(DependSuffix) -MM quaternion.cpp

$(IntermediateDirectory)/quaternion.cpp$(PreprocessSuffix): quaternion.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/quaternion.cpp$(PreprocessSuffix) quaternion.cpp

$(IntermediateDirectory)/Renderer.cpp$(ObjectSuffix): Renderer.cpp $(IntermediateDirectory)/Renderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Renderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Renderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Renderer.cpp$(DependSuffix): Renderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Renderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Renderer.cpp$(DependSuffix) -MM Renderer.cpp

$(IntermediateDirectory)/Renderer.cpp$(PreprocessSuffix): Renderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Renderer.cpp$(PreprocessSuffix) Renderer.cpp

$(IntermediateDirectory)/Scene.cpp$(ObjectSuffix): Scene.cpp $(IntermediateDirectory)/Scene.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Scene.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Scene.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Scene.cpp$(DependSuffix): Scene.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Scene.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Scene.cpp$(DependSuffix) -MM Scene.cpp

$(IntermediateDirectory)/Scene.cpp$(PreprocessSuffix): Scene.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Scene.cpp$(PreprocessSuffix) Scene.cpp

$(IntermediateDirectory)/Shader.cpp$(ObjectSuffix): Shader.cpp $(IntermediateDirectory)/Shader.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Shader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Shader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Shader.cpp$(DependSuffix): Shader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Shader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Shader.cpp$(DependSuffix) -MM Shader.cpp

$(IntermediateDirectory)/Shader.cpp$(PreprocessSuffix): Shader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Shader.cpp$(PreprocessSuffix) Shader.cpp

$(IntermediateDirectory)/SkyboxRenderer.cpp$(ObjectSuffix): SkyboxRenderer.cpp $(IntermediateDirectory)/SkyboxRenderer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/SkyboxRenderer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/SkyboxRenderer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/SkyboxRenderer.cpp$(DependSuffix): SkyboxRenderer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/SkyboxRenderer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/SkyboxRenderer.cpp$(DependSuffix) -MM SkyboxRenderer.cpp

$(IntermediateDirectory)/SkyboxRenderer.cpp$(PreprocessSuffix): SkyboxRenderer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/SkyboxRenderer.cpp$(PreprocessSuffix) SkyboxRenderer.cpp

$(IntermediateDirectory)/Sphere.cpp$(ObjectSuffix): Sphere.cpp $(IntermediateDirectory)/Sphere.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Sphere.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Sphere.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Sphere.cpp$(DependSuffix): Sphere.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Sphere.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Sphere.cpp$(DependSuffix) -MM Sphere.cpp

$(IntermediateDirectory)/Sphere.cpp$(PreprocessSuffix): Sphere.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Sphere.cpp$(PreprocessSuffix) Sphere.cpp

$(IntermediateDirectory)/Texture.cpp$(ObjectSuffix): Texture.cpp $(IntermediateDirectory)/Texture.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Texture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Texture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Texture.cpp$(DependSuffix): Texture.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Texture.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Texture.cpp$(DependSuffix) -MM Texture.cpp

$(IntermediateDirectory)/Texture.cpp$(PreprocessSuffix): Texture.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Texture.cpp$(PreprocessSuffix) Texture.cpp

$(IntermediateDirectory)/Transform.cpp$(ObjectSuffix): Transform.cpp $(IntermediateDirectory)/Transform.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Transform.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Transform.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Transform.cpp$(DependSuffix): Transform.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Transform.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Transform.cpp$(DependSuffix) -MM Transform.cpp

$(IntermediateDirectory)/Transform.cpp$(PreprocessSuffix): Transform.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Transform.cpp$(PreprocessSuffix) Transform.cpp

$(IntermediateDirectory)/Triangle.cpp$(ObjectSuffix): Triangle.cpp $(IntermediateDirectory)/Triangle.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Triangle.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Triangle.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Triangle.cpp$(DependSuffix): Triangle.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Triangle.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Triangle.cpp$(DependSuffix) -MM Triangle.cpp

$(IntermediateDirectory)/Triangle.cpp$(PreprocessSuffix): Triangle.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Triangle.cpp$(PreprocessSuffix) Triangle.cpp

$(IntermediateDirectory)/DefaultScript.cpp$(ObjectSuffix): DefaultScript.cpp $(IntermediateDirectory)/DefaultScript.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/DefaultScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/DefaultScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/DefaultScript.cpp$(DependSuffix): DefaultScript.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/DefaultScript.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/DefaultScript.cpp$(DependSuffix) -MM DefaultScript.cpp

$(IntermediateDirectory)/DefaultScript.cpp$(PreprocessSuffix): DefaultScript.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/DefaultScript.cpp$(PreprocessSuffix) DefaultScript.cpp

$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix): GameObject.cpp $(IntermediateDirectory)/GameObject.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/GameObject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/GameObject.cpp$(DependSuffix): GameObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/GameObject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/GameObject.cpp$(DependSuffix) -MM GameObject.cpp

$(IntermediateDirectory)/GameObject.cpp$(PreprocessSuffix): GameObject.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/GameObject.cpp$(PreprocessSuffix) GameObject.cpp

$(IntermediateDirectory)/LauncherScript.cpp$(ObjectSuffix): LauncherScript.cpp $(IntermediateDirectory)/LauncherScript.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/LauncherScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LauncherScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LauncherScript.cpp$(DependSuffix): LauncherScript.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LauncherScript.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LauncherScript.cpp$(DependSuffix) -MM LauncherScript.cpp

$(IntermediateDirectory)/LauncherScript.cpp$(PreprocessSuffix): LauncherScript.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LauncherScript.cpp$(PreprocessSuffix) LauncherScript.cpp

$(IntermediateDirectory)/RotateInPlaceScript.cpp$(ObjectSuffix): RotateInPlaceScript.cpp $(IntermediateDirectory)/RotateInPlaceScript.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/RotateInPlaceScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/RotateInPlaceScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/RotateInPlaceScript.cpp$(DependSuffix): RotateInPlaceScript.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/RotateInPlaceScript.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/RotateInPlaceScript.cpp$(DependSuffix) -MM RotateInPlaceScript.cpp

$(IntermediateDirectory)/RotateInPlaceScript.cpp$(PreprocessSuffix): RotateInPlaceScript.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/RotateInPlaceScript.cpp$(PreprocessSuffix) RotateInPlaceScript.cpp

$(IntermediateDirectory)/Mesh.cpp$(ObjectSuffix): Mesh.cpp $(IntermediateDirectory)/Mesh.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Mesh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Mesh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Mesh.cpp$(DependSuffix): Mesh.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Mesh.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Mesh.cpp$(DependSuffix) -MM Mesh.cpp

$(IntermediateDirectory)/Mesh.cpp$(PreprocessSuffix): Mesh.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Mesh.cpp$(PreprocessSuffix) Mesh.cpp

$(IntermediateDirectory)/RotateAboutPointScript.cpp$(ObjectSuffix): RotateAboutPointScript.cpp $(IntermediateDirectory)/RotateAboutPointScript.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/RotateAboutPointScript.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/RotateAboutPointScript.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/RotateAboutPointScript.cpp$(DependSuffix): RotateAboutPointScript.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/RotateAboutPointScript.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/RotateAboutPointScript.cpp$(DependSuffix) -MM RotateAboutPointScript.cpp

$(IntermediateDirectory)/RotateAboutPointScript.cpp$(PreprocessSuffix): RotateAboutPointScript.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/RotateAboutPointScript.cpp$(PreprocessSuffix) RotateAboutPointScript.cpp

$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(ObjectSuffix): include/jasper/AssetSerializer.cpp $(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/include/jasper/AssetSerializer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(DependSuffix): include/jasper/AssetSerializer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(DependSuffix) -MM include/jasper/AssetSerializer.cpp

$(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(PreprocessSuffix): include/jasper/AssetSerializer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/include_jasper_AssetSerializer.cpp$(PreprocessSuffix) include/jasper/AssetSerializer.cpp

$(IntermediateDirectory)/imgui.cpp$(ObjectSuffix): imgui.cpp $(IntermediateDirectory)/imgui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/imgui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/imgui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/imgui.cpp$(DependSuffix): imgui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/imgui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/imgui.cpp$(DependSuffix) -MM imgui.cpp

$(IntermediateDirectory)/imgui.cpp$(PreprocessSuffix): imgui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/imgui.cpp$(PreprocessSuffix) imgui.cpp

$(IntermediateDirectory)/imgui_draw.cpp$(ObjectSuffix): imgui_draw.cpp $(IntermediateDirectory)/imgui_draw.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/imgui_draw.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/imgui_draw.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/imgui_draw.cpp$(DependSuffix): imgui_draw.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/imgui_draw.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/imgui_draw.cpp$(DependSuffix) -MM imgui_draw.cpp

$(IntermediateDirectory)/imgui_draw.cpp$(PreprocessSuffix): imgui_draw.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/imgui_draw.cpp$(PreprocessSuffix) imgui_draw.cpp

$(IntermediateDirectory)/Scriptable.cpp$(ObjectSuffix): Scriptable.cpp $(IntermediateDirectory)/Scriptable.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/Scriptable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Scriptable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Scriptable.cpp$(DependSuffix): Scriptable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/Scriptable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/Scriptable.cpp$(DependSuffix) -MM Scriptable.cpp

$(IntermediateDirectory)/Scriptable.cpp$(PreprocessSuffix): Scriptable.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Scriptable.cpp$(PreprocessSuffix) Scriptable.cpp

$(IntermediateDirectory)/ParticleSystem.cpp$(ObjectSuffix): ParticleSystem.cpp $(IntermediateDirectory)/ParticleSystem.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/ParticleSystem.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ParticleSystem.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ParticleSystem.cpp$(DependSuffix): ParticleSystem.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ParticleSystem.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ParticleSystem.cpp$(DependSuffix) -MM ParticleSystem.cpp

$(IntermediateDirectory)/ParticleSystem.cpp$(PreprocessSuffix): ParticleSystem.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ParticleSystem.cpp$(PreprocessSuffix) ParticleSystem.cpp

$(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(ObjectSuffix): ForsythIndexOptimizer.cpp $(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/al/code/testapp/testapp/ForsythIndexOptimizer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(DependSuffix): ForsythIndexOptimizer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(DependSuffix) -MM ForsythIndexOptimizer.cpp

$(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(PreprocessSuffix): ForsythIndexOptimizer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ForsythIndexOptimizer.cpp$(PreprocessSuffix) ForsythIndexOptimizer.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


