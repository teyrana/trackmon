#!/bin/bash

# this script runs the dockerfile from the .devcontainer folder
# !!! WARNING: This script is hardcoded to this repo !!!
#     (But it should be easy enough to adapt ;)

# See Also:
# - For a full-featured, more flexible version:
#   https://blog.wille-zone.de/post/run-devcontainer-outside-of-visual-studio-code/

# local paths (host system)
CONTEXT_PATH=${PWD}

# container paths (container / client system)
WORKSPACE_PATH=/workspaces
PROJECT_PATH=${WORKSPACE_PATH}/trackmon

DOCKER_IMAGE_TAG=trackmon
DOCKER_IMAGE_LABEL=devel

# echo "    << ${DOCKER_IMAGE_HASH}"
# echo "    << ${DOCKER_IMAGE_TAG}"
# echo "    << ${DOCKER_IMAGE_LABEL}"

MOUNT_ARGS="type=bind,source=${CONTEXT_PATH},destination=${PROJECT_PATH}"
docker run -it --rm -w $PROJECT_PATH --mount=${MOUNT_ARGS} ${DOCKER_IMAGE_TAG}:${DOCKER_IMAGE_LABEL}
