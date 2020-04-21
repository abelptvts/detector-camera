docker run -it --rm --user $(id -u):$(id -g) -v $(pwd):/usr/app -w /usr/app/ -e DOCKER=1 detector-camera-build-arm /bin/bash
