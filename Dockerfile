FROM ubuntu as build
RUN apt-get update && \
	apt-get install -y git python3 && \
	mkdir /build && \
	cd /build && \
	git clone https://github.com/emscripten-core/emsdk.git
RUN cd /build/emsdk && \
	./emsdk install latest && \
	./emsdk activate latest
RUN DEBIAN_FRONTEND="noninteractive" apt-get install -y libxml2-dev
RUN apt-get install -y make
COPY ./src/game /project
COPY ./Makefile /project
ENV PATH="${PATH}:/build/emsdk/upstream/bin"
RUN cd /project && \
	chmod +x /build/emsdk/emsdk_env.sh && \
	make compile-game

FROM scratch AS export
COPY --from=build /project/public/game.wasm .
