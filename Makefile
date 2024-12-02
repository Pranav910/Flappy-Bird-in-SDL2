TARGET_DIR = ./build

all: create_build_dir build_source run_source

create_build_dir:
	@powershell -Command "if (-not (Test-Path '$(TARGET_DIR)')) { \
		Write-Host ''; \
		Write-Host ''; \
		Write-Host '-> Could not find target directory, creating one...'; \
		New-Item -ItemType Directory -Path '$(TARGET_DIR)'; \
		Write-Host ''; \
		Write-Host ''; \
		Write-Host '-> Successfully created target directory.'; \
	} else { \
		Write-Host ''; \
		Write-Host ''; \
		Write-Host '-> Directory $(TARGET_DIR) already exists.'; \
	}"

build_source:
	@powershell -Command "Write-Host ''; Write-Host '-> Compiling source files...'; Write-Host ''; \
		Write-Host '-> g++ -I src/include -L src/lib -o $(TARGET_DIR)/main main.cpp -lmingw32 -lSDL2_image -lSDL2main -lSDL2'; \
		g++ -I src/include -L src/lib -o $(TARGET_DIR)/main main.cpp -lmingw32 -lSDL2_image -lSDL2main -lSDL2 -lSDL2_mixer; \
		Write-Host ''; Write-Host '-> Source files compiled successfully.'"

run_source:
	@powershell -Command "Write-Host ''; Write-Host '-> Running target file...'; Write-Host ''; \
		Write-Host '-> $(TARGET_DIR)/main'; \
		& $(TARGET_DIR)/main"
