#!/bin/bash

# This script attaches to the docker container from the .devcontainer folder
# !!! WARNING: This script is hardcoded to this repo !!!
#     (But it should be easy enough to adapt ;)
 
# local paths (host system)
CONTEXT_PATH=${PWD}
DOCKER_FILE_PATH=${CONTEXT_PATH}/.devcontainer/Dockerfile

# container paths (container / client system)
WORKSPACE_PATH=/workspaces
PROJECT_PATH=${WORKSPACE_PATH}/trackmon
REMOTE_USER="vscode"
DOCKER_CONTAINER_NAME=track-mon-vscode-dev
MOUNT_ARGS="type=bind,source=${CONTEXT_PATH},destination=${PROJECT_PATH}"

echo ">> Attaching to container :${DOCKER_CONTAINER_NAME}"
docker exec -it -w $PROJECT_PATH ${DOCKER_CONTAINER_NAME} $SHELL
