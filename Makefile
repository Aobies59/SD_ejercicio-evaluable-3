# Define the compiler
CC = gcc

# Shared library location (current folder)
LIB_LOCATION = ./

# All source files
CLIENT_SRC = client.c
CLIENT = client
SERVER = server
RPCGEN = rpcgen
SOURCES.x = server.x

# RPC Server files
TARGETS_SVC.c = server_svc.c server.c server_xdr.c
TARGET_CLNT.c = server_clnt.c
TARGETS = server.h server_xdr.c server_clnt.c server_svc.c server_client.c server.c

OBJECTS_SVC = $(TARGETS_SVC.c:%.c=%.o)

# Compiler flags
CFLAGS = -Wall -g -Wno-unused-variable
LDFLAGS = -lrt -pthread -lnsl -lpthread -ltirpc
RPCGENFLAGS = -CNM
LIBFLAGS = -Wl,-rpath=./ -lclaves

all:
	@make --no-print-directory clean
	@make --no-print-directory $(SERVER)
	@make --no-print-directory $(CLIENT)

# Target to generate the server executable with rpcgen
#	@cp backup/server_server.c .
$(RPCGEN): $(SOURCES.x)
	@rpcgen $(RPCGENFLAGS) $(SOURCES.x)
	@rpcgen -CNM -s tcp server.x > server_svc.c
	@echo "generated RPC files"

$(TARGETS_SVC.c) : $(RPCGEN)
$(OBJECTS_SVC) : $(TARGETS_SVC.c) 

# Target to build the server executable
$(SERVER): $(OBJECTS_SVC) $(RPCGEN)
	@$(LINK.c) -o $(SERVER) $(OBJECTS_SVC)
	@echo "compiled server"

# Target to build the client executable, creating and using the shared library libclaves.so
$(CLIENT): $(RPCGEN)
	@$(LINK.c) -shared -fPIC claves.c $(TARGETS_SVC.c) $(TARGET_CLNT.c) -o libclaves.so $(CFLAGS) $(LDFLAGS)
	@$(LINK.c) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC) $(LDFLAGS) -L$(LIB_LOCATION) $(LIBFLAGS)
	@echo "compiled client"

# Clean rule to remove all object files and executables
clean:
	@rm -f server client libclaves.so
	@rm -f server_*
	@rm -f server.h $(SERVER)
