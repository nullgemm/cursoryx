#!/bin/bash

# example for a release build
# $ ./run.sh /scripts/build_appkit.sh release appkit osxcross
docker run \
	--privileged \
	--name cursoryx_container_appkit_osxcross \
	-e AR=x86_64-apple-darwin20.2-ar \
	cursoryx_image_appkit_osxcross "$@" &>> log
