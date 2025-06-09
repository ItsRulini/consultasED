#pragma once
#include <Windows.h>
#include <fstream>
#include "CommCtrl.h"
#include "ctime"
#include "resource.h"


struct Usuario {
	int idUsuario; // PK, autoincremental
	char usuario[50]; // Nombre de usuario UNIQUE
	char contrasena[50];
	bool estatus; // Activo o inactivo

	Usuario* siguiente;
	Usuario* anterior;
};

Usuario* primerUsuario = NULL, * ultimoUsuario = NULL;
Usuario* auxUsuario = NULL;
char archivoUsuarios[MAX_PATH];

int generarIdUsuario() {
	if (ultimoUsuario == NULL) {
		return 1; // Si no hay usuarios, empezamos en 1
	}
	return ultimoUsuario->idUsuario + 1; // El ID es el del �ltimo + 1
}

Usuario* crearUsuario(char* usuario, char* contrase�a, bool estatus) {
	Usuario* nuevoUsuario = new Usuario;
	nuevoUsuario->idUsuario = generarIdUsuario(); // Funci�n para generar un ID �nico
	strcpy(nuevoUsuario->usuario, usuario);
	strcpy(nuevoUsuario->contrasena, contrase�a);
	nuevoUsuario->estatus = estatus;
	nuevoUsuario->siguiente = NULL;
	nuevoUsuario->anterior = NULL;
	return nuevoUsuario;
}

void agregarUsuario(Usuario* nuevo) {
	if (primerUsuario == NULL) {
		primerUsuario = nuevo;
		ultimoUsuario = nuevo;
	}
	else {
		ultimoUsuario->siguiente = nuevo;
		nuevo->anterior = ultimoUsuario;
		ultimoUsuario = nuevo;
	}
}

Usuario* buscarUsuario(char* usuario) {
	Usuario* actual = primerUsuario;
	while (actual != NULL) {
		if (strcmp(actual->usuario, usuario) == 0) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

Usuario* buscarUsuario(char* usuario, char* contrase�a) {
	Usuario* actual = primerUsuario;
	while (actual != NULL) {
		if (strcmp(actual->usuario, usuario) == 0 && strcmp(actual->contrasena, contrase�a) == 0) {
			// Usuario encontrado con las credenciales correctas
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

Usuario* buscarUsuarioPorId(int id) {
	Usuario* actual = primerUsuario;
	while (actual != NULL) {
		if (actual->idUsuario == id) {
			return actual;
		}
		actual = actual->siguiente;
	}
	return NULL;
}

void llenarListaUsuarios(HWND hListBox) {
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	Usuario* actual = primerUsuario;
	while (actual != NULL) {
		const char* estatusStr = actual->estatus ? "Activo" : "Inactivo";
		char buffer[150];
		sprintf_s(buffer, "%d | %s | Estatus: %s", actual->idUsuario, actual->usuario, estatusStr);
		SendMessageA(hListBox, LB_ADDSTRING, 0, (LPARAM)buffer);
		actual = actual->siguiente;
	}
}

void escribirUsuarios(std::ofstream& archivo) {
	Usuario* actual = primerUsuario;
	while (actual != nullptr)
	{
		archivo.write(reinterpret_cast<const char*>(actual), sizeof(Usuario));
		actual = actual->siguiente;
	}
}

void leerUsuarios(std::ifstream& archivo) {
	while (true)
	{
		Usuario* nuevo = new Usuario;
		archivo.read(reinterpret_cast<char*>(nuevo), sizeof(Usuario));

		if (archivo.eof())
		{
			delete nuevo; // libreando la memoria que creamos y no utilizamos del puntero 'nuevo'
			break;
		}

		agregarUsuario(nuevo);
	}
}