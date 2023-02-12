FROM ubuntu:22.04
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Moscow apt-get -y install build-essential pkg-config wget zip unzip curl tar git
RUN wget -O /tmp/vcpkg.tar.gz https://github.com/microsoft/vcpkg/archive/refs/tags/2023.01.09.tar.gz
RUN mkdir -p /opt/vcpkg
RUN tar xf /tmp/vcpkg.tar.gz --strip-components=1 -C /opt/vcpkg
RUN /opt/vcpkg/bootstrap-vcpkg.sh
RUN ln -s /opt/vcpkg/vcpkg /usr/local/bin/vcpkg
RUN rm -rf /tmp/vcpkg.tar.gz
ENV VCPKG_ROOT /opt/vcpkg
