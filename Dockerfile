#Build Stage
FROM --platform=linux/amd64 ubuntu:20.04 as builder

##Install Build Dependencies
RUN apt-get update && \
        DEBIAN_FRONTEND=noninteractive apt-get install -y git clang libwebp-dev libgdk-pixbuf2.0-dev libgtk-3-dev meson build-essential ninja-build

##ADD source code to the build stage
WORKDIR /
ADD https://api.github.com/repos/ennamarie19/webp-pixbuf-loader/git/refs/heads/mayhem version.json
RUN git clone -b mayhem https://github.com/ennamarie19/webp-pixbuf-loader.git
WORKDIR /webp-pixbuf-loader


##Build
RUN mkdir build
RUN CC=clang CXX=clang++ CFLAGS=-fsanitize=fuzzer-no-link meson setup build -Dgdk_build_for_fuzz=enabled -Dgdk_pixbuf_query_loaders_path=/usr/lib/x86_64-linux-gnu/gdk-pixbuf-2.0/gdk-pixbuf-query-loaders
RUN ninja -v -C build -j$(nproc)

##Prepare all library dependencies for copy
#RUN mkdir /deps
#RUN cp `ldd ./test/.libs/fuzz | grep so | sed -e '/^[^\t]/ d' | sed -e 's/\t//' | sed -e 's/.*=..//' | sed -e 's/ (0.*)//' | sort | uniq` /deps 2>/dev/null || :
#
#FROM --platform=linux/amd64 ubuntu:20.04
#COPY --from=builder /openslide/test/.libs/fuzz /fuzz
#COPY --from=builder /deps /usr/lib

#CMD /fuzz

