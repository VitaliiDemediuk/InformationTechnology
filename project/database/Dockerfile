FROM gcc:12.2.0

RUN apt-get update 

# install boost
RUN cd /home && wget http://downloads.sourceforge.net/project/boost/boost/1.77.0/boost_1_77_0.tar.gz \
  && tar xfz boost_1_77_0.tar.gz \
  && rm boost_1_77_0.tar.gz \
  && cd boost_1_77_0 \
  && ./bootstrap.sh --prefix=/usr/local --with-libraries=program_options \
  && ./b2 install \
  && cd /home \
  && rm -rf boost_1_77_0

ENV LD_LIBRARY_PATH /usr/local/lib

# install cmake
ARG CMAKE_VERSION=3.25.0
RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-Linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh

ENV PATH="/usr/bin/cmake/bin:${PATH}"

# install proto & grpc
ENV MY_INSTALL_DIR=/home/.local
RUN mkdir -p $MY_INSTALL_DIR
ENV PATH="$MY_INSTALL_DIR/bin:$PATH"
RUN apt install -y build-essential autoconf libtool pkg-config
RUN cd $MY_INSTALL_DIR
ENV GRPC_SOURCE_DIR ${MY_INSTALL_DIR}/grpc
RUN git clone --recurse-submodules -b v1.50.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc ${GRPC_SOURCE_DIR}
ENV GRPC_BUILD_DIR ${GRPC_SOURCE_DIR}/cmake/build
RUN mkdir -p ${GRPC_BUILD_DIR}
RUN cmake -DgRPC_INSTALL=ON \
      -DgRPC_BUILD_TESTS=OFF \
      -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
      -S ${GRPC_SOURCE_DIR} \
      -B ${GRPC_BUILD_DIR}
RUN make -j 4 --directory=${GRPC_BUILD_DIR}
RUN make --directory=${GRPC_BUILD_DIR} install

### install mongodbcxx driver

# installing the mongoc dependencies and driver
RUN apt-get install -y \
    pkg-config \
    libssl-dev \
    libsasl2-dev

RUN cd ~ \
    && wget https://github.com/mongodb/mongo-c-driver/releases/download/1.23.1/mongo-c-driver-1.23.1.tar.gz \
    && tar xzf mongo-c-driver-1.23.1.tar.gz \
    && cd mongo-c-driver-1.23.1 \
    && mkdir cmake-build \
    && cd cmake-build \
    && cmake -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF .. \
    && cmake --build . \
    && cmake --build . --target install

#installing mongocxx driver - connects c++ to mongo
RUN cd ~ \
    && curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.6.0/mongo-cxx-driver-r3.6.0.tar.gz \
    && tar -xzf mongo-cxx-driver-r3.6.0.tar.gz \
    && cd mongo-cxx-driver-r3.6.0/build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. \
    && make EP_mnmlstc_core \
    && make \
    && make install \
    && cd ~ \
    && rm mongo-cxx-driver-r3.6.0.tar.gz \
    && rm -rf mongo-cxx-driver-r3.6.0


ENV BUILD_PATH $HOME/grpc_database_server

ADD cmake $BUILD_PATH/src/cmake
ADD core $BUILD_PATH/src/core
ADD gRpcServer $BUILD_PATH/src/gRpcServer
ADD proto $BUILD_PATH/src/proto
ADD thirdparty $BUILD_PATH/src/thirdparty
ADD custom_test_db_projects $BUILD_PATH/src/custom_test_db_projects

WORKDIR $BUILD_PATH/src/gRpcServer

RUN mkdir build \
    && cd build \
    && cmake .. || true

RUN cd build \
    && cmake .. \
    && make

ENTRYPOINT export LD_LIBRARY_PATH=/usr/local/lib \
           && ./build/gRpcDatabaseServer --load-type="custom" --database-file="../custom_test_db_projects/my.cdb" --server-ip="0.0.0.0" --server-port="50051"