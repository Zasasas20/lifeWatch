FROM python:3

WORKDIR /app

# Copies script 
COPY script.py ./

RUN apt-get -y update && \
    apt-get -y upgrade && \
    apt-get install -y unixodbc-dev unixodbc odbcinst && \
    apt-get install -y odbc-postgresql odbc-mariadb libsqliteodbc libssl-dev && \
    pip install pipreqs && \
    pipreqs /app && \
    pip install --no-cache-dir -r requirements.txt && \
    apt-get install apt-transport-https && \
    curl https://packages.microsoft.com/keys/microsoft.asc | apt-key add - && \
    curl https://packages.microsoft.com/config/ubuntu/$(lsb_release -rs)/prod.list > /etc/apt/sources.list.d/mssql-release.list && \
    apt-get update && apt install -y apt-utils

# INSTALL ODBC DRIVER
RUN ACCEPT_EULA=Y apt-get install -y msodbcsql17

# Runs script, default entry point /bin/sh -c
CMD [ "python", "./script.py" ]
