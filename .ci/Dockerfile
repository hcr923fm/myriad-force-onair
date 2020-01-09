FROM cirrusci/windowsservercore:cmake
RUN choco upgrade all -y
#RUN choco upgrade -y visualstudio2017community visualstudio2017-workload-vctools
RUN choco install -y visualstudio2017-workload-nativedesktop

WORKDIR C:/
RUN git clone https://github.com/Microsoft/vcpkg.git
WORKDIR vcpkg
RUN .\bootstrap-vcpkg.bat
RUN .\vcpkg integrate install
RUN .\vcpkg install boost-program-options:x86-windows boost-asio:x86-windows rapidjson:x86-windows