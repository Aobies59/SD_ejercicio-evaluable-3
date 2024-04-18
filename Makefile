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
TARGETS_SVC.c = server_svc.c server_server.c server_xdr.c 
TARGETS = server.h server_xdr.c server_clnt.c server_svc.c server_client.c server_server.c

OBJECTS_SVC = $(TARGETS_SVC.c:%.c=%.o)

# Compiler flags
CFLAGS = -Wall -g
LDFLAGS = -lrt -pthread -lnsl -lpthread -ltirpc
LDLIBS += -lnsl -lpthread -ltirpc
RPCGENFLAGS = -CNMa
LIBFLAGS = -Wl,-rpath=./ -lclaves


all:
	@make --no-print-directory $(RPCGEN)
	@make --no-print-directory $(SERVER)
	@make --no-print-directory $(CLIENT)

# Target to generate the server executable with rpcgen
$(RPCGEN): $(SOURCES.x)
	@rpcgen $(RPCGENFLAGS) $(SOURCES.x)
	@cp backup/server_server.c .
	@echo "generated server"

$(OBJECTS_SVC) : $(TARGETS_SVC.c) 

# Target to build the server executable
$(SERVER): $(OBJECTS_SVC) 
	@$(LINK.c) -o server $(OBJECTS_SVC) $(LDLIBS)
	@echo "compiled server"

# Target to build the client executable, creating and using the shared library libclaves.so
$(CLIENT):
	@$(CC) -shared -fPIC claves.c -o libclaves.so $(CFLAGS) $(LDFLAGS)
	@$(CC) $(CFLAGS) -o client $(CLIENT_SRC) $(LDFLAGS) -L$(LIB_LOCATION) $(LIBFLAGS)
	@echo "compiled client"

# Clean rule to remove all object files and executables
clean:
	@rm -f server client libclaves.so
	@rm $(TARGETS) $(OBJECTS_SVC) $(SERVER)
