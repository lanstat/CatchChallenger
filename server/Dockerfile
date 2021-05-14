FROM debian:buster-slim AS base
WORKDIR /app

FROM debian:buster-slim AS build
WORKDIR /src
COPY ["server/", "server/"]
COPY ["general/", "general/"]
RUN apt-get update && apt-get install -y build-essential gcc automake qt5-qmake libzstd-dev zlib1g-dev libssl-dev libpq-dev qttools5-dev qt5-default
WORKDIR "/src/server"
RUN qmake catchchallenger-server-cli-epoll.pro 
RUN make

FROM base AS final
WORKDIR /app
COPY --from=build /src/server/catchchallenger-server-cli-epoll .
RUN chmod a+x /app/catchchallenger-server-cli-epoll
RUN apt-get update && apt-get install -y libpq5 
CMD /app/catchchallenger-server-cli-epoll
