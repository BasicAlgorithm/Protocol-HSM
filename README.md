# HIGH SPEED MOSQUETEROS PROTOCOL
# Introduccion

- This protocol is an introductory knowledge about internet protocols

# Login
### Definition
This action is about

### Structure [user 🡒 server]
```c++
{
	char accion; // l 
	char tamaño_user[2];
	char tamaño_password[2];
	char* user; 
	char* password; 
}
```
>Example:
l0504julioucsp
l1104santistebanucsp

### Structure [server 🡒 user]
```c++
ok {
	char accion; // L
	char ok[2];
}
```
>Example:
Lok
E fix-20 msg

# Lista
### Definition
This action is about

### Structure [user 🡒 server]
```c++
{
	char accion; // i
}
```

### Structure [server 🡒 user]
```c++
{
	char accion; // I
	char num_users[2];
	char tamaño_user_name[2];
	char* user_name; 
}
```
>Example: I03110305SantistebanLeePeter
    I03030203JonWuLee

# Message to user
### Definition
This action is about

### Structure [user 🡒 server]
```c++
{
	char accion; // m
	char tamaño_msg[3];
	char tamaño_destinatario[2];
	char* msg;
	char* destinatario;
}
```

### Structure [server 🡒 user]
```c++
{
	char accion; // M
	char tamaño_msg[3];
	char tamaño_remitente[2];
	char* msg;
	char* remitente;
}
```

# Broadcast Message
### Definition
This action is about

### Structure [user 🡒 server]
```c++
{
	char accion; // b
	char tamaño_msg[3];
	char* msg;
}
```

### Structure [server 🡒 user]
```c++
{
	char accion; // B
	char tamaño_msg[3];
	char tamaño_remitente[2];
	char* msg;
	char* remitente;
}
```

# Upload File
### Definition
This action is about

### Structure [user 🡒 server]
```c++
{
	char accion;
	char tamaño_file_name[3];
	char tamaño_file_data[10];
	char tamaño_destinatario[2];
	char* file_name;
	char* file_data;
	char* destinatario;
	
}
```

### Structure [server 🡒 user]
```c++
{
	char accion; // U
	char tamaño_file_name[3];
	char tamaño_file_data[10];
	char tamaño_remitente[2];
	char* file_name;
	char* file_data;
	char* remitente;
	
}
```

# Accept or reject File
### Definition
This action is about

### Structure [user 🡒 server]
```c++
{
	char accion; // f	
	char tamaño_remitente[2];
	char* remitente;
}
```

### Structure [server 🡒 user]
```c++
{
	char accion; // F	remitente notificado que rechazaron archivo
	char tamaño_user_name[2];
	char* user_name;
}
```


# Exit chat
### Definition
This action is about

### Structure [user 🡒 server]
```c++
{
	char accion; // x
}
```

### Structure [server 🡒 user]
```c++
{
	char accion; // X
}
```

# Error
### Definition
This action is about

### Structure [server 🡒 user]
```c++
{
	char accion; // E
	char msg[20];
}
```
