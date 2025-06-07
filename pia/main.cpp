#include "Medico.h"
#include "Paciente.h"
#include "Especialidad.h"
#include "Consultorio.h"
#include "Cita.h"

// Declarar el prototipo de la función ventanaInicio
LRESULT CALLBACK vInicio(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vDoctor(HWND, UINT, WPARAM, LPARAM); 
LRESULT CALLBACK vPaciente(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vConsultorio(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK vAgendarCitas(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK ventanaInicio(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK ventanaInicio(HWND, UINT, WPARAM, LPARAM);
//LRESULT CALLBACK ventanaInicio(HWND, UINT, WPARAM, LPARAM);

// Archivos binarios
void InicializarArchivos();
void leerArchivosBinarios();
void guardarArchivosBinarios();

// Menú
void menu(HWND hwnd, WPARAM wParam);

// Funciones para llenar combo box
int CompareSystemTime(SYSTEMTIME t1, SYSTEMTIME t2);
//void llenarComboEspecialidades(HWND hwndCombo, Especialidad* nodo);

// Variables globales
bool registro = false;
bool modificar = false;

HINSTANCE hInstGlobal;

int WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int cShow) {
	InicializarArchivos();
	leerArchivosBinarios();
	/*HWND hVentanaInicial = */
	DialogBox(hInst, MAKEINTRESOURCE(IDD_MAIN_WINDOW), NULL, vInicio);
	MSG msg; //Variable para los mensajes recibidos en la ventana
	ZeroMemory(&msg, sizeof(MSG));

	//ShowWindow(hVentanaInicial, cShow);

	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK vInicio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{
	case WM_COMMAND: {

		switch (LOWORD(wParam)) {
		case INGRESAR_BTN: {

			HWND hUsuario = GetDlgItem(hwnd, IDC_USUARIO_ADMIN);
			HWND hContrasena = GetDlgItem(hwnd, IDC_PASS_ADMIN);
			char usuario[50];
			char contrasena[50];

			GetWindowText(hUsuario, usuario, sizeof(usuario));
			GetWindowText(hContrasena, contrasena, sizeof(contrasena));

			if (strcmp(usuario, "admin") == 0 && strcmp(contrasena, "admin") == 0) {
				EndDialog(hwnd, 0);
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PACIENTE), NULL, vPaciente);
			}
			else {
				MessageBox(NULL, "Usuario o contraseña incorrectos", "Error", MB_OK | MB_ICONERROR);
			}

		} break;

		}

	} break;
	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}

	return FALSE;
}


LRESULT CALLBACK vDoctor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Medico* medicoSeleccionado = NULL;

	switch (msg)
	{
	case WM_INITDIALOG: {
		modificar = false;
		HWND hComboEspecialidad = GetDlgItem(hwnd, COMBO_ESPECIALIDAD);
		SendMessage(hComboEspecialidad, CB_RESETCONTENT, 0, 0); // Limpia por si acaso
		llenarComboEspecialidades(hComboEspecialidad, raiz); // Llenar desde la raíz del árbol
		HWND hListMedicos = GetDlgItem(hwnd, LIST_INFO_MEDICOS);
		llenarListaMedicos(hListMedicos, primeroMedico); // donde cabezaMedicos es el inicio de la lista enlazada
	} break;

	case WM_COMMAND: {
		menu(hwnd, wParam);
		switch (LOWORD(wParam)) {
		case APLICAR_MEDICO_BTN: {
			HWND hNombreMedico = GetDlgItem(hwnd, IDC_NOMBRES_INFO_MEDICO);
			HWND hPaternoMedico = GetDlgItem(hwnd, IDC_PATERNO_INFO_MEDICO);
			HWND hMaternoMedico = GetDlgItem(hwnd, IDC_MATERNO_INFO_MEDICO);
			HWND hCedula = GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO);
			HWND hEmail = GetDlgItem(hwnd, IDC_EMAIL_INFO_MEDICO);
			HWND hTelefono = GetDlgItem(hwnd, IDC_TEL_INFO_MEDICO);
			HWND hEspecialidad = GetDlgItem(hwnd, COMBO_ESPECIALIDAD);
			HWND hActivo = GetDlgItem(hwnd, IDC_ACTIVO_MEDICO);
			HWND hInactivo = GetDlgItem(hwnd, IDC_INACTIVO_MEDICO);

			char nombre[50], paterno[50], materno[50], correo[50], buffer[20];
			int cedula = 0;
			long telefono = 0;

			// Obtener textos
			GetWindowTextA(hNombreMedico, nombre, 50);
			GetWindowTextA(hPaternoMedico, paterno, 50);
			GetWindowTextA(hMaternoMedico, materno, 50);
			GetWindowTextA(hCedula, buffer, 20);

			if (strlen(nombre) == 0) {
				MessageBox(hwnd, "El nombre del médico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (strlen(paterno) == 0) {
				MessageBox(hwnd, "El apellido paterno del médico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (strlen(materno) == 0) {
				MessageBox(hwnd, "El apellido materno del médico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Validar que la cédula no esté vacía
			if (strlen(buffer) == 0 && !modificar) {
				MessageBox(hwnd, "La cédula no puede estar vacía.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (sscanf(buffer, "%d", &cedula) != 1 && !modificar) {
				MessageBox(hwnd, "Cédula inválida. Debe ser un número entero.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Validar que el teléfono no esté vacío

			GetWindowTextA(hTelefono, buffer, 20);
			if (strlen(buffer) == 0) {
				MessageBox(hwnd, "El teléfono no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			// Validar que el correo no esté vacío
			GetWindowTextA(hEmail, correo, 50);
			if (strlen(correo) == 0) {
				MessageBox(hwnd, "El correo no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			GetWindowTextA(hTelefono, buffer, 20);
			if (sscanf(buffer, "%ld", &telefono) != 1) {
				MessageBox(hwnd, "Teléfono inválido. Debe ser un número.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			GetWindowTextA(hEmail, correo, 50);

			// Validar selección en el ComboBox de especialidad
			int index = SendMessage(hEspecialidad, CB_GETCURSEL, 0, 0);
			

			if (index == CB_ERR) {
				MessageBox(hwnd, "Seleccione una especialidad.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			// Obtener el ID de la especialidad (puede ser el mismo índice o asociado con CB_GETITEMDATA si lo manejas)
			//int idEspecialidad = index; // ajusta si usas CB_SETITEMDATA / CB_GETITEMDATA
			int idEspecialidad = SendMessage(hEspecialidad, CB_GETITEMDATA, index, 0);

			// Validar estatus (activo o inactivo)
			bool estatus = false;
			if (SendMessage(hActivo, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				estatus = true;
			}
			else if (SendMessage(hInactivo, BM_GETCHECK, 0, 0) == BST_CHECKED) {
				estatus = false;
			}
			else {
				MessageBox(hwnd, "Seleccione un estatus para el médico (Activo/Inactivo).", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Verificar si ya existe un médico con la misma cédula
			if (buscarMedico(cedula) != NULL) {
				MessageBox(hwnd, "Ya existe un médico con esta cédula.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (medicoSeleccionado != NULL) {
				// Modificar médico existente
				strcpy(medicoSeleccionado->nombre, nombre);
				strcpy(medicoSeleccionado->apellidoPaterno, paterno);
				strcpy(medicoSeleccionado->apellidoMaterno, materno);
				strcpy(medicoSeleccionado->correo, correo);
				medicoSeleccionado->telefono = telefono;
				medicoSeleccionado->idEspecialidad = idEspecialidad;
				medicoSeleccionado->estatus = estatus;

				MessageBox(hwnd, "Médico modificado correctamente.", "Modificado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				// Validar que no exista un médico con ese ID
				if (buscarMedico(cedula)) {
					MessageBox(hwnd, "Ya existe un médico con este ID.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				// Crear y agregar médico a la lista
				Medico* nuevo = crearMedico(cedula, nombre, paterno, materno, correo, telefono, idEspecialidad, estatus);
				agregarMedico(nuevo);
				MessageBox(hwnd, "Médico agregado correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
			}

			// Crear y agregar médico a la lista
			/*Medico* nuevo = crearMedico(cedula, nombre, paterno, materno, correo, telefono, idEspecialidad, estatus);
			agregarMedico(nuevo);*/

			llenarListaMedicos(GetDlgItem(hwnd, LIST_INFO_MEDICOS), primeroMedico);

		} break;
		case BUSCAR_MEDICO_BTN: {
			HWND hCedula = GetDlgItem(hwnd, IDC_BUSCAR_MEDICO);
			char buffer[20];
			GetWindowTextA(hCedula, buffer, 20);

			if (strlen(buffer) == 0) {
				MessageBox(hwnd, "Ingrese la cédula del médico a buscar.", "Advertencia", MB_OK | MB_ICONWARNING);
				break;
			}

			int cedula = 0;
			if (sscanf(buffer, "%d", &cedula) != 1) {
				MessageBox(hwnd, "Cédula inválida.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			medicoSeleccionado = buscarMedico(cedula);
			if (medicoSeleccionado == NULL) {
				MessageBox(hwnd, "No se encontró el médico.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			modificar = true; // Cambiar el estado a modificar
			// Llenar campos
			EnableWindow(GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO), FALSE);
			SetWindowTextA(GetDlgItem(hwnd, IDC_NOMBRES_INFO_MEDICO), medicoSeleccionado->nombre);
			SetWindowTextA(GetDlgItem(hwnd, IDC_PATERNO_INFO_MEDICO), medicoSeleccionado->apellidoPaterno);
			SetWindowTextA(GetDlgItem(hwnd, IDC_MATERNO_INFO_MEDICO), medicoSeleccionado->apellidoMaterno);
			SetWindowTextA(GetDlgItem(hwnd, IDC_EMAIL_INFO_MEDICO), medicoSeleccionado->correo);

			char tel[20];
			sprintf(tel, "%ld", medicoSeleccionado->telefono);
			SetWindowTextA(GetDlgItem(hwnd, IDC_TEL_INFO_MEDICO), tel);

			// Seleccionar especialidad en combo
			HWND hCombo = GetDlgItem(hwnd, COMBO_ESPECIALIDAD);
			int count = SendMessage(hCombo, CB_GETCOUNT, 0, 0);
			for (int i = 0; i < count; i++) {
				int id = SendMessage(hCombo, CB_GETITEMDATA, i, 0);
				if (id == medicoSeleccionado->idEspecialidad) {
					SendMessage(hCombo, CB_SETCURSEL, i, 0);
					break;
				}
			}

			// Seleccionar estatus
			CheckRadioButton(hwnd, IDC_ACTIVO_MEDICO, IDC_INACTIVO_MEDICO,
				medicoSeleccionado->estatus ? IDC_ACTIVO_MEDICO : IDC_INACTIVO_MEDICO);
		} break;

		case IDC_LIMPIAR_MEDICO: {
			SetWindowTextA(GetDlgItem(hwnd, IDC_NOMBRES_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_PATERNO_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_MATERNO_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_EMAIL_INFO_MEDICO), "");
			SetWindowTextA(GetDlgItem(hwnd, IDC_TEL_INFO_MEDICO), "");
			modificar = false; // Reiniciar el estado de modificación
			medicoSeleccionado = NULL; // Reiniciar el médico seleccionado
			// Desmarcar estatus
			CheckRadioButton(hwnd, IDC_ACTIVO_MEDICO, IDC_INACTIVO_MEDICO, 0);
			EnableWindow(GetDlgItem(hwnd, IDC_CEDULA_INFO_MEDICO), TRUE);
			// Reiniciar ComboBox
			SendMessage(GetDlgItem(hwnd, COMBO_ESPECIALIDAD), CB_SETCURSEL, -1, 0);
		} break;

		}

	} break;
	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK vPaciente(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Paciente* pacienteActual = NULL; // Para guardar el paciente encontrado y modificarlo
	switch (msg)
	{
		case WM_INITDIALOG: {
			modificar = false;
			HWND hListPacientes = GetDlgItem(hwnd, LIST_INFO_PACIENTES);
			llenarListaPacientes(hListPacientes, primeroPaciente); // donde cabezaPacientes es el inicio de la lista enlazada
		} break;
		case WM_COMMAND: {
			menu(hwnd, wParam);
			switch (LOWORD(wParam)) {
			case APLICAR_PACIENTE_BTN: {
				HWND hNombrePaciente = GetDlgItem(hwnd, NOMBRES_INFO_PACIENTE);
				HWND hPaternoPaciente = GetDlgItem(hwnd, PATERNO_INFO_PACIENTE);
				HWND hMaternoPaciente = GetDlgItem(hwnd, MATERNO_INFO_PACIENTE);
				HWND hIdPaciente = GetDlgItem(hwnd, ID_INFO_PACIENTE);
				HWND hEmail = GetDlgItem(hwnd, EMAIL_INFO_PACIENTE);
				HWND hTelefono = GetDlgItem(hwnd, TEL_INFO_PACIENTE);
				HWND hFechaNac = GetDlgItem(hwnd, NACIMIENTO_INFO_PACIENTE);
				HWND hSexoM = GetDlgItem(hwnd, HOMBRE_PACIENTE);
				HWND hSexoF = GetDlgItem(hwnd, MUJER_PACIENTE);
				HWND hActivo = GetDlgItem(hwnd, ACTIVO_PACIENTE);
				HWND hInactivo = GetDlgItem(hwnd, INACTIVO_PACIENTE);

				char nombre[50], paterno[50], materno[50], correo[50], buffer[20];
				int idPaciente = 0;
				long telefono = 0;
				SYSTEMTIME fechaNacimiento;

				// Obtener textos
				GetWindowTextA(hNombrePaciente, nombre, 50);
				GetWindowTextA(hPaternoPaciente, paterno, 50);
				GetWindowTextA(hMaternoPaciente, materno, 50);
				GetWindowTextA(hIdPaciente, buffer, 20);

				if (strlen(buffer) == 0 && !modificar) {
					MessageBox(hwnd, "El id del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				if (sscanf(buffer, "%d", &telefono) != 1 && !modificar) {
					MessageBox(hwnd, "Id inválido.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				GetWindowTextA(hEmail, correo, 50);
				GetWindowTextA(hTelefono, buffer, 20);

				if (strlen(buffer) == 0) {
					MessageBox(hwnd, "El teléfono del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				if (sscanf(buffer, "%d", &telefono) != 1) {
					MessageBox(hwnd, "Teléfono inválido.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				telefono = atol(buffer);

				// Validaciones de campos
				if (strlen(nombre) == 0) {
					MessageBox(hwnd, "El nombre del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (strlen(paterno) == 0) {
					MessageBox(hwnd, "El apellido paterno del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (strlen(materno) == 0) {
					MessageBox(hwnd, "El apellido materno del paciente no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (strlen(correo) == 0) {
					MessageBox(hwnd, "El correo electrónico no puede estar vacío.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				
				if (!IsDlgButtonChecked(hwnd, HOMBRE_PACIENTE) && !IsDlgButtonChecked(hwnd, MUJER_PACIENTE)) {
					MessageBox(hwnd, "Debe seleccionar un sexo para el paciente.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				if (!IsDlgButtonChecked(hwnd, ACTIVO_PACIENTE) && !IsDlgButtonChecked(hwnd, INACTIVO_PACIENTE)) {
					MessageBox(hwnd, "Debe seleccionar un estatus para el paciente.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				// Obtener la fecha del DateTimePicker
				DateTime_GetSystemtime(hFechaNac, &fechaNacimiento);

				// Validar que la fecha no sea mayor a la de ayer
				SYSTEMTIME hoy, ayer;
				GetLocalTime(&hoy);
				ayer = hoy;
				if (ayer.wDay > 1) {
					ayer.wDay--;
				}
				else {
					if (ayer.wMonth == 1) {
						ayer.wYear--;
						ayer.wMonth = 12;
					}
					else {
						ayer.wMonth--;
					}
					// Esto es una simplificación. Para precisión real, deberías usar funciones de tiempo.
					ayer.wDay = 28; // Aproximación aceptable
				}

				if (CompareSystemTime(fechaNacimiento, ayer) > 0) {
					MessageBox(hwnd, "La fecha de nacimiento debe ser anterior al día de ayer.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				// Sexo y estatus
				SEXO sexo = IsDlgButtonChecked(hwnd, HOMBRE_PACIENTE) ? MASCULINO : FEMENINO;
				bool estatus = IsDlgButtonChecked(hwnd, ACTIVO_PACIENTE);

				// ID de paciente
				GetWindowTextA(hIdPaciente, buffer, 20);
				idPaciente = atoi(buffer);

				if (!modificar) {
					if (buscarPaciente(idPaciente)) {
						MessageBox(hwnd, "Ya existe un paciente con ese ID.", "Error", MB_OK | MB_ICONERROR);
						break;
					}
					Paciente* nuevo = crearPaciente(idPaciente, nombre, paterno, materno, correo, telefono, fechaNacimiento, sexo, estatus);
					agregarPaciente(nuevo);

					MessageBox(hwnd, "Paciente agregado exitosamente.", "Información", MB_OK | MB_ICONINFORMATION);
				}
				else {
					// Modo modificar
					//static Paciente* pacienteMod = NULL;
					if (pacienteActual) {
						strcpy(pacienteActual->nombre, nombre);
						strcpy(pacienteActual->apellidoPaterno, paterno);
						strcpy(pacienteActual->apellidoMaterno, materno);
						strcpy(pacienteActual->correo, correo);
						pacienteActual->telefono = telefono;
						pacienteActual->fechaNacimiento = fechaNacimiento;
						pacienteActual->sexo = sexo;
						pacienteActual->estatus = estatus;

						MessageBox(hwnd, "Paciente modificado correctamente.", "Información", MB_OK | MB_ICONINFORMATION);
					}
					modificar = false;
				}

				// Actualizar lista y limpiar campos (opcional)
				llenarListaPacientes(GetDlgItem(hwnd, LIST_INFO_PACIENTES), primeroPaciente);

			} break;

			case BUSCAR_PACIENTE_BTN: {
				char buffer[20];
				GetWindowTextA(GetDlgItem(hwnd, IDC_BUSCAR_PACIENTE), buffer, 20);
				int id = atoi(buffer);
				pacienteActual = buscarPaciente(id);
				if (pacienteActual == NULL) {
					MessageBox(hwnd, "Paciente no encontrado.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				EnableWindow(GetDlgItem(hwnd, ID_INFO_PACIENTE), FALSE);

				char idPaciente[20];
				sprintf_s(idPaciente, "%d", pacienteActual->idPaciente);

				SetWindowTextA(GetDlgItem(hwnd, ID_INFO_PACIENTE), idPaciente);
				SetWindowTextA(GetDlgItem(hwnd, NOMBRES_INFO_PACIENTE), pacienteActual->nombre);
				SetWindowTextA(GetDlgItem(hwnd, PATERNO_INFO_PACIENTE), pacienteActual->apellidoPaterno);
				SetWindowTextA(GetDlgItem(hwnd, MATERNO_INFO_PACIENTE), pacienteActual->apellidoMaterno);
				SetWindowTextA(GetDlgItem(hwnd, EMAIL_INFO_PACIENTE), pacienteActual->correo);
				sprintf_s(buffer, "%ld", pacienteActual->telefono);
				SetWindowTextA(GetDlgItem(hwnd, TEL_INFO_PACIENTE), buffer);
				DateTime_SetSystemtime(GetDlgItem(hwnd, NACIMIENTO_INFO_PACIENTE), GDT_VALID, &pacienteActual->fechaNacimiento);
				CheckRadioButton(hwnd, HOMBRE_PACIENTE, MUJER_PACIENTE, pacienteActual->sexo == MASCULINO ? HOMBRE_PACIENTE : MUJER_PACIENTE);
				CheckRadioButton(hwnd, ACTIVO_PACIENTE, INACTIVO_PACIENTE, pacienteActual->estatus ? ACTIVO_PACIENTE : INACTIVO_PACIENTE);
				modificar = true;
			} break;

			case IDC_LIMPIAR_PACIENTE: {
				SetWindowTextA(GetDlgItem(hwnd, NOMBRES_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, PATERNO_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, MATERNO_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, ID_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, EMAIL_INFO_PACIENTE), "");
				SetWindowTextA(GetDlgItem(hwnd, TEL_INFO_PACIENTE), "");
				EnableWindow(GetDlgItem(hwnd, ID_INFO_PACIENTE), TRUE);
				DateTime_SetSystemtime(GetDlgItem(hwnd, NACIMIENTO_INFO_PACIENTE), GDT_NONE, NULL);
				CheckRadioButton(hwnd, HOMBRE_PACIENTE, MUJER_PACIENTE, 0);
				CheckRadioButton(hwnd, ACTIVO_PACIENTE, INACTIVO_PACIENTE, 0);
				modificar = false;
				pacienteActual = NULL;
			} break;

			case CITA_BTN: {

				if(!modificar || pacienteActual == NULL) {
					MessageBox(hwnd, "Debe buscar o seleccionar un paciente antes de agendar una cita.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				auxPaciente = pacienteActual; // Guardar el paciente actual para usarlo en la ventana de agendar citas
				EndDialog(hwnd, 0);
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_AGENDAR_CITAS), NULL, vAgendarCitas);

			} break;

			}
			break;
		case WM_CLOSE: {
			guardarArchivosBinarios();
			PostQuitMessage(0);
		} break;
		}
	}
		return FALSE;
	
}

// Somewhere globally or accessible if needed (usually static within the function is fine)
// Paciente* auxPaciente; // This should be set before calling DialogBox for vAgendarCitas

LRESULT CALLBACK vAgendarCitas(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Consultorio* consultorioSeleccionadoParaAgendar = NULL;
	static Medico* medicoDelConsultorioSeleccionado = NULL;
	static SYSTEMTIME fechaHoraDeCitaPropuesta; // Stores date and time selected by user

	switch (msg)
	{
	case WM_INITDIALOG: {
		consultorioSeleccionadoParaAgendar = NULL;
		medicoDelConsultorioSeleccionado = NULL;

		// Llenar el combo de especialidades
		HWND hComboEsp = GetDlgItem(hwnd, COMBO_ESPECIALIDAD_AGENDAR);
		SendMessage(hComboEsp, CB_RESETCONTENT, 0, 0);
		llenarComboEspecialidades(hComboEsp, raiz); //

		// Llenar la información del paciente (auxPaciente should be valid here)
		if (auxPaciente != NULL) {
			llenarInfoPaciente(hwnd, auxPaciente); // Function to fill patient info in the dialog
		}
		else {
			MessageBox(hwnd, "Error: No se ha seleccionado un paciente.", "Error Paciente", MB_OK | MB_ICONERROR);
			EndDialog(hwnd, 0); // Close if no patient
		}

		// Set default time for DTPs to current time
		SYSTEMTIME stLocalTime;
		GetLocalTime(&stLocalTime);
		DateTime_SetSystemtime(GetDlgItem(hwnd, FECHA_CITA_AGENDAR), GDT_VALID, &stLocalTime);
		DateTime_SetSystemtime(GetDlgItem(hwnd, HORA_CITA_AGENDAR), GDT_VALID, &stLocalTime);

		return TRUE; // Important for WM_INITDIALOG
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case COMBO_ESPECIALIDAD_AGENDAR: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				// Clear previously listed consultorios and selected doctor info
				HWND hListConsultorios = GetDlgItem(hwnd, LIST_CONSULTORIOS); // ID 1045
				SendMessage(hListConsultorios, LB_RESETCONTENT, 0, 0);
				consultorioSeleccionadoParaAgendar = NULL;
				medicoDelConsultorioSeleccionado = NULL;
				SetDlgItemTextA(hwnd, STATIC_CEDULA_MEDICO, ""); // Assumed ID 1081
				SetDlgItemTextA(hwnd, NOMBRE_MEDICO_AGENDAR, "");    // ID 1046
				SetDlgItemTextA(hwnd, PATERNO_MEDICO_AGENDAR, "");   // ID 1047
				SetDlgItemTextA(hwnd, MATERNO_MEDICO_AGENDAR, "");   // ID 1048
			}
			break;
		}
		case DISPONIBILIDAD_CITA_BTN: { // Assumed ID 1080
			HWND hComboEsp = GetDlgItem(hwnd, COMBO_ESPECIALIDAD_AGENDAR);
			int selEspIndex = SendMessage(hComboEsp, CB_GETCURSEL, 0, 0);
			if (selEspIndex == CB_ERR) {
				MessageBox(hwnd, "Por favor, seleccione una especialidad.", "Error", MB_OK | MB_ICONWARNING);
				break;
			}
			int idEspecialidadSeleccionada = SendMessage(hComboEsp, CB_GETITEMDATA, selEspIndex, 0);

			SYSTEMTIME fechaSeleccionada, horaSeleccionada, hoy;
			GetLocalTime(&hoy); // For date validation
			hoy.wHour = 0; hoy.wMinute = 0; hoy.wSecond = 0; hoy.wMilliseconds = 0; // Compare dates only

			DateTime_GetSystemtime(GetDlgItem(hwnd, FECHA_CITA_AGENDAR), &fechaSeleccionada);
			DateTime_GetSystemtime(GetDlgItem(hwnd, HORA_CITA_AGENDAR), &horaSeleccionada);

			fechaHoraDeCitaPropuesta = fechaSeleccionada;
			fechaHoraDeCitaPropuesta.wHour = horaSeleccionada.wHour;
			fechaHoraDeCitaPropuesta.wMinute = horaSeleccionada.wMinute;
			fechaHoraDeCitaPropuesta.wSecond = horaSeleccionada.wSecond;
			fechaHoraDeCitaPropuesta.wMilliseconds = 0; // Clear milliseconds for comparison

			// Validate date is not in the past
			SYSTEMTIME tempFechaPropuestaForDateCheck = fechaHoraDeCitaPropuesta;
			tempFechaPropuestaForDateCheck.wHour = 0; tempFechaPropuestaForDateCheck.wMinute = 0; tempFechaPropuestaForDateCheck.wSecond = 0;

			if (CompareSystemTime(tempFechaPropuestaForDateCheck, hoy) < 0) { //
				MessageBox(hwnd, "No se pueden agendar citas en fechas pasadas.", "Error de Fecha", MB_OK | MB_ICONWARNING);
				break;
			}

			// Validate time is not in the past if date is today
			if (CompareSystemTime(tempFechaPropuestaForDateCheck, hoy) == 0) {
				SYSTEMTIME tiempoActual;
				GetLocalTime(&tiempoActual);
				if (CompareSystemTime(fechaHoraDeCitaPropuesta, tiempoActual) < 0) {
					MessageBox(hwnd, "No se pueden agendar citas en horas pasadas para el día de hoy.", "Error de Hora", MB_OK | MB_ICONWARNING);
					break;
				}
			}


			HWND hListConsultorios = GetDlgItem(hwnd, LIST_CONSULTORIOS); // ID 1045
			SendMessage(hListConsultorios, LB_RESETCONTENT, 0, 0);
			consultorioSeleccionadoParaAgendar = NULL; // Reset selection
			medicoDelConsultorioSeleccionado = NULL;
			SetDlgItemTextA(hwnd, STATIC_CEDULA_MEDICO, "");
			SetDlgItemTextA(hwnd, NOMBRE_MEDICO_AGENDAR, "");
			SetDlgItemTextA(hwnd, PATERNO_MEDICO_AGENDAR, "");
			SetDlgItemTextA(hwnd, MATERNO_MEDICO_AGENDAR, "");

			Consultorio* consultorioActual = primeroConsultorio;
			bool foundAvailable = false;
			while (consultorioActual != NULL) {
				Medico* medicoAsignado = buscarMedico(consultorioActual->cedulaMedico); //
				if (medicoAsignado != NULL && medicoAsignado->estatus == true && medicoAsignado->idEspecialidad == idEspecialidadSeleccionada) {
					// Check availability of this consultorio at fechaHoraDeCitaPropuesta
					bool disponible = true;
					Cita* citaExistente = primeraCita;
					while (citaExistente != NULL) {
						if (citaExistente->idConsultorio == consultorioActual->idConsultorio && citaExistente->estatus == PENDIENTE) {
							SYSTEMTIME inicioCitaExistente = citaExistente->fechaHoraCita;
							SYSTEMTIME finCitaExistente = citaExistente->fechaHoraCita;
							AddSecondsToSystemTime(&finCitaExistente, 30); // Citas duran 30 segundos

							SYSTEMTIME inicioNuevaCita = fechaHoraDeCitaPropuesta;
							SYSTEMTIME finNuevaCita = fechaHoraDeCitaPropuesta;
							AddSecondsToSystemTime(&finNuevaCita, 30);

							// Check for overlap: (StartA < EndB) and (EndA > StartB)
							if (CompareSystemTime(inicioNuevaCita, finCitaExistente) < 0 && CompareSystemTime(finNuevaCita, inicioCitaExistente) > 0) {
								disponible = false;
								break;
							}
						}
						citaExistente = citaExistente->siguiente;
					}

					if (disponible) {
						foundAvailable = true;
						char buffer[256];
						sprintf_s(buffer, sizeof(buffer), "Consultorio %d - Dr. %s %s", consultorioActual->idConsultorio, medicoAsignado->nombre, medicoAsignado->apellidoPaterno);
						int index = SendMessageA(hListConsultorios, LB_ADDSTRING, 0, (LPARAM)buffer);
						SendMessage(hListConsultorios, LB_SETITEMDATA, index, (LPARAM)consultorioActual->idConsultorio);
					}
				}
				consultorioActual = consultorioActual->siguiente;
			}
			if (!foundAvailable) {
				MessageBox(hwnd, "No hay consultorios disponibles para la especialidad y horario seleccionados.", "Disponibilidad", MB_OK | MB_ICONINFORMATION);
			}
			break;
		}

		case LIST_CONSULTORIOS: { // ID 1045, treated as ListBox of available Consultorios
			if (HIWORD(wParam) == LBN_SELCHANGE) {
				HWND hListConsultorios = GetDlgItem(hwnd, LIST_CONSULTORIOS);
				int selIndex = SendMessage(hListConsultorios, LB_GETCURSEL, 0, 0);
				if (selIndex != LB_ERR) {
					int idConsultorio = SendMessage(hListConsultorios, LB_GETITEMDATA, selIndex, 0);
					consultorioSeleccionadoParaAgendar = buscarConsultorio(primeroConsultorio, idConsultorio); //

					if (consultorioSeleccionadoParaAgendar != NULL) {
						medicoDelConsultorioSeleccionado = buscarMedico(consultorioSeleccionadoParaAgendar->cedulaMedico); //
						if (medicoDelConsultorioSeleccionado != NULL) {
							char cedulaStr[20];
							sprintf_s(cedulaStr, sizeof(cedulaStr), "%d", medicoDelConsultorioSeleccionado->cedula);
							SetDlgItemTextA(hwnd, STATIC_CEDULA_MEDICO, cedulaStr); // Assumed ID 1081
							SetDlgItemTextA(hwnd, NOMBRE_MEDICO_AGENDAR, medicoDelConsultorioSeleccionado->nombre);       // ID 1046
							SetDlgItemTextA(hwnd, PATERNO_MEDICO_AGENDAR, medicoDelConsultorioSeleccionado->apellidoPaterno); // ID 1047
							SetDlgItemTextA(hwnd, MATERNO_MEDICO_AGENDAR, medicoDelConsultorioSeleccionado->apellidoMaterno); // ID 1048
						}
						else {
							// Medico not found (should not happen if data is consistent)
							consultorioSeleccionadoParaAgendar = NULL; // Invalidate selection
							medicoDelConsultorioSeleccionado = NULL;
							SetDlgItemTextA(hwnd, STATIC_CEDULA_MEDICO, "Error");
							SetDlgItemTextA(hwnd, NOMBRE_MEDICO_AGENDAR, "Médico no encontrado");
							SetDlgItemTextA(hwnd, PATERNO_MEDICO_AGENDAR, "");
							SetDlgItemTextA(hwnd, MATERNO_MEDICO_AGENDAR, "");
						}
					}
					else {
						// Consultorio not found (should not happen if list is populated correctly)
						consultorioSeleccionadoParaAgendar = NULL;
						medicoDelConsultorioSeleccionado = NULL;
						SetDlgItemTextA(hwnd, STATIC_CEDULA_MEDICO, "Error");
						SetDlgItemTextA(hwnd, NOMBRE_MEDICO_AGENDAR, "Consultorio no encontrado");
						SetDlgItemTextA(hwnd, PATERNO_MEDICO_AGENDAR, "");
						SetDlgItemTextA(hwnd, MATERNO_MEDICO_AGENDAR, "");
					}
				}
			}
			break;
		}

		case AGENDAR_CITA_BTN: { // ID 1043
			if (auxPaciente == NULL) {
				MessageBox(hwnd, "Error: No hay información del paciente.", "Error", MB_OK | MB_ICONERROR);
				break;
			}
			if (consultorioSeleccionadoParaAgendar == NULL || medicoDelConsultorioSeleccionado == NULL) {
				MessageBox(hwnd, "Por favor, seleccione un consultorio disponible de la lista.", "Error", MB_OK | MB_ICONWARNING);
				break;
			}

			SYSTEMTIME fechaFinalCita, horaFinalCita, fechaHoraFinalParaCita, hoy;
			GetLocalTime(&hoy); // For date validation
			hoy.wHour = 0; hoy.wMinute = 0; hoy.wSecond = 0; hoy.wMilliseconds = 0;

			DateTime_GetSystemtime(GetDlgItem(hwnd, FECHA_CITA_AGENDAR), &fechaFinalCita);
			DateTime_GetSystemtime(GetDlgItem(hwnd, HORA_CITA_AGENDAR), &horaFinalCita);

			fechaHoraFinalParaCita = fechaFinalCita;
			fechaHoraFinalParaCita.wHour = horaFinalCita.wHour;
			fechaHoraFinalParaCita.wMinute = horaFinalCita.wMinute;
			fechaHoraFinalParaCita.wSecond = horaFinalCita.wSecond;
			fechaHoraFinalParaCita.wMilliseconds = 0;

			// Critical Validations (again, in case state changed)
			SYSTEMTIME tempFechaPropuestaForDateCheck = fechaHoraFinalParaCita;
			tempFechaPropuestaForDateCheck.wHour = 0; tempFechaPropuestaForDateCheck.wMinute = 0; tempFechaPropuestaForDateCheck.wSecond = 0;

			if (CompareSystemTime(tempFechaPropuestaForDateCheck, hoy) < 0) { //
				MessageBox(hwnd, "No se pueden agendar citas en fechas pasadas.", "Error de Fecha", MB_OK | MB_ICONWARNING);
				break;
			}
			if (CompareSystemTime(tempFechaPropuestaForDateCheck, hoy) == 0) {
				SYSTEMTIME tiempoActual;
				GetLocalTime(&tiempoActual);
				if (CompareSystemTime(fechaHoraFinalParaCita, tiempoActual) < 0) {
					MessageBox(hwnd, "No se pueden agendar citas en horas pasadas para el día de hoy.", "Error de Hora", MB_OK | MB_ICONWARNING);
					break;
				}
			}


			// Re-check for conflicts for the selected consultorio at the chosen time
			bool conflicto = false;
			Cita* citaExistente = primeraCita;
			while (citaExistente != NULL) {
				if (citaExistente->idConsultorio == consultorioSeleccionadoParaAgendar->idConsultorio && citaExistente->estatus == PENDIENTE) {
					SYSTEMTIME inicioCitaExistente = citaExistente->fechaHoraCita;
					SYSTEMTIME finCitaExistente = citaExistente->fechaHoraCita;
					AddSecondsToSystemTime(&finCitaExistente, 30);

					SYSTEMTIME inicioNuevaCita = fechaHoraFinalParaCita;
					SYSTEMTIME finNuevaCita = fechaHoraFinalParaCita;
					AddSecondsToSystemTime(&finNuevaCita, 30);

					if (CompareSystemTime(inicioNuevaCita, finCitaExistente) < 0 && CompareSystemTime(finNuevaCita, inicioCitaExistente) > 0) {
						conflicto = true;
						break;
					}
				}
				citaExistente = citaExistente->siguiente;
			}

			if (conflicto) {
				MessageBox(hwnd, "El horario seleccionado ya no está disponible. Por favor, verifique la disponibilidad nuevamente.", "Conflicto de Horario", MB_OK | MB_ICONWARNING);
				// Optionally, refresh the availability list here
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(DISPONIBILIDAD_CITA_BTN, BN_CLICKED), (LPARAM)GetDlgItem(hwnd, DISPONIBILIDAD_CITA_BTN));
				break;
			}

			// Proceed to create and add the appointment
			char diagnosticoTemp[MAX_PATH] = "Pendiente";
			Cita* nuevaCita = crearCita(
				medicoDelConsultorioSeleccionado->cedula,
				auxPaciente->idPaciente,
				consultorioSeleccionadoParaAgendar->idConsultorio,
				diagnosticoTemp,
				fechaHoraFinalParaCita,
				PENDIENTE); //

			if (nuevaCita) {
				agregarCita(nuevaCita); //
				MessageBox(hwnd, "Cita agendada exitosamente.", "Éxito", MB_OK | MB_ICONINFORMATION);

				// Clear selections and fields
				consultorioSeleccionadoParaAgendar = NULL;
				medicoDelConsultorioSeleccionado = NULL;
				SetDlgItemTextA(hwnd, STATIC_CEDULA_MEDICO, "");
				SetDlgItemTextA(hwnd, NOMBRE_MEDICO_AGENDAR, "");
				SetDlgItemTextA(hwnd, PATERNO_MEDICO_AGENDAR, "");
				SetDlgItemTextA(hwnd, MATERNO_MEDICO_AGENDAR, "");
				SendMessage(GetDlgItem(hwnd, LIST_CONSULTORIOS), LB_RESETCONTENT, 0, 0); // Clear the list
				SendMessage(GetDlgItem(hwnd, LIST_CONSULTORIOS), LB_SETCURSEL, (WPARAM)-1, 0); // Deselect
				SendMessage(GetDlgItem(hwnd, COMBO_ESPECIALIDAD_AGENDAR), CB_SETCURSEL, (WPARAM)-1, 0); // Deselect specialty

				// Optionally, close dialog or allow scheduling another for same patient
				// EndDialog(hwnd, 0); // If you want to close after scheduling
				// DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PACIENTE), NULL, vPaciente);
			}
			else {
				MessageBox(hwnd, "Error al crear la cita.", "Error", MB_OK | MB_ICONERROR);
			}
			break;
		}
		}
	} break;
	case WM_CLOSE: {
		auxPaciente = NULL; // Limpiar el paciente auxiliar
		EndDialog(hwnd, 0); // Cerrar la ventana de citas
		// Decide if you want to reopen vPaciente or another dialog. The original code did.
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PACIENTE), NULL, vPaciente);
	} break;
	default:
		return FALSE; // Let Windows handle other messages
	}
	return TRUE; // We processed the message
}


LRESULT CALLBACK ventanaEspecialidad(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Especialidad* espSeleccionada = NULL; // Puntero persistente

	switch (msg)
	{
	case WM_INITDIALOG: {
		modificar = false;
		llenarListaEspecialidades(GetDlgItem(hwnd, LIST_INFO_PACIENTES));
	} break;

	case WM_COMMAND: {
		menu(hwnd, wParam);

		switch (LOWORD(wParam)) {

		case APLICAR_ESP_BTN: {
			HWND hIdEspecialidad = GetDlgItem(hwnd, ID_INFO_ESP);
			HWND hNombreEsp = GetDlgItem(hwnd, NOMBRE_INFO_ESP);

			int idEspecialidad = 0;
			char nombre[50];
			char buffer[20];

			GetWindowTextA(hNombreEsp, nombre, 50);
			GetWindowTextA(hIdEspecialidad, buffer, 20);

			if (strlen(nombre) == 0 || strlen(buffer) == 0 && !modificar) {
				MessageBox(hwnd, "Todos los campos son obligatorios.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (sscanf(buffer, "%d", &idEspecialidad) != 1 && !modificar) {
				MessageBox(hwnd, "ID inválido. Debe ser un número.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (espSeleccionada != NULL) {
				// Modificar la especialidad existente
				//espSeleccionada->idEspecialidad = idEspecialidad;
				strcpy(espSeleccionada->nombre, nombre);
				MessageBox(hwnd, "Especialidad modificada correctamente.", "Modificado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				// Validar que no exista ese ID
				if (buscarEspecialidad(raiz, idEspecialidad)) {
					MessageBox(hwnd, "Ya existe una especialidad con este ID.", "Error", MB_OK | MB_ICONERROR);
					break;
				}
				// Crear y agregar nueva especialidad
				Especialidad* nueva = crearEspecialidad(idEspecialidad, nombre);
				agregarEspecialidad(raiz, nueva);
				MessageBox(hwnd, "Especialidad agregada correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
			}

			//espSeleccionada = NULL; // Reiniciar después de aplicar
			llenarListaEspecialidades(GetDlgItem(hwnd, LIST_INFO_PACIENTES));
		} break;

		case BUSCAR_ESP_BTN: {
			char buffer[20];
			HWND hIdEspecialidad = GetDlgItem(hwnd, IDC_BUSCAR_ESP);
			GetWindowTextA(hIdEspecialidad, buffer, 20);
			int idBuscado = 0;

			if (sscanf(buffer, "%d", &idBuscado) != 1) {
				MessageBox(hwnd, "Ingresa un ID válido.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			espSeleccionada = buscarEspecialidad(raiz, idBuscado);
			if (espSeleccionada) {
				SetDlgItemTextA(hwnd, NOMBRE_INFO_ESP, espSeleccionada->nombre);
				modificar = true; // Cambiar el estado a modificar
				EnableWindow(GetDlgItem(hwnd, ID_INFO_ESP), FALSE);
				MessageBox(hwnd, "Especialidad encontrada. Puedes modificarla.", "Encontrado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				MessageBox(hwnd, "Especialidad no encontrada.", "Error", MB_OK | MB_ICONERROR);
			}
		} break;

		case LIMPIAR_ESP_BTN: {
			EnableWindow(GetDlgItem(hwnd, ID_INFO_ESP), TRUE);
			SetDlgItemTextA(hwnd, ID_INFO_ESP, "");
			SetDlgItemTextA(hwnd, NOMBRE_INFO_ESP, "");
			modificar = false; // Reiniciar el estado de modificación
			espSeleccionada = NULL; // Limpiar selección
		} break;
		}
	} break;

	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK vConsultorio(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static Consultorio* consultorioSeleccionado = NULL; // Puntero persistente

	switch (msg)
	{
	case WM_INITDIALOG: {
		modificar = false;
		llenarComboMedicos(hwnd);
		llenarListaConsultorios(hwnd);
	} break;

	case WM_COMMAND: {
		menu(hwnd, wParam);

		switch (LOWORD(wParam)) {

			// ===== MANEJAR SELECCIÓN EN COMBO DE MÉDICOS =====
		case COMBO_MEDICOS: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				int cedulaMedico = obtenerCedulaDelCombo(hwnd);
				if (cedulaMedico != -1) {
					llenarEspecialidadDelMedico(hwnd, cedulaMedico);
				}
			}
		} break;

						  // ===== BUSCAR CONSULTORIO =====
		case BTN_BUSCAR_CONSULTORIO: {
			
			char buffer[20];
			HWND hBuscar = GetDlgItem(hwnd, BUSCAR_CONSULTORIO);
			GetWindowTextA(hBuscar, buffer, 20);
			int idBuscado = 0;

			if (sscanf(buffer, "%d", &idBuscado) != 1) {
				MessageBox(hwnd, "Ingresa un ID válido.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			consultorioSeleccionado = buscarConsultorio(primeroConsultorio, idBuscado);
			if (consultorioSeleccionado) {
				//llenarComboMedicos(hwnd); // Asegurarse de que el combo de médicos esté lleno antes de buscar
				// Solo medicos que tengan la especialidad del consultorio
				int especialidadId = 0;
				especialidadId = buscarMedico(consultorioSeleccionado->cedulaMedico)->idEspecialidad;
				llenarComboMedicosEspecialidadConsultorio(hwnd, especialidadId); // llenar combo de médicos por especialidad antes de buscar
				// Llenar campos con datos del consultorio encontrado
				SetDlgItemInt(hwnd, IDC_ID_CONSULTORIO, consultorioSeleccionado->idConsultorio, FALSE);

				// Seleccionar el médico en el combo
				HWND hCombo = GetDlgItem(hwnd, COMBO_MEDICOS);
				int items = SendMessage(hCombo, CB_GETCOUNT, 0, 0);



				bool doctorInactivo = true;

				for (int i = 0; i < items; i++) {
					char buffer[100];
					SendMessageA(hCombo, CB_GETLBTEXT, i, (LPARAM)buffer);
					int cedula = 0;
					if (sscanf(buffer, "%d", &cedula) == 1 && cedula == consultorioSeleccionado->cedulaMedico) {
						SendMessage(hCombo, CB_SETCURSEL, i, 0);
						// Llenar especialidad automáticamente
						doctorInactivo = false; // Si se encuentra, no es inactivo
						llenarEspecialidadDelMedico(hwnd, cedula);
						break;
					}
				}

				if (doctorInactivo) {
					MessageBox(hwnd, "El médico asignado al consultorio está inactivo.", "Advertencia", MB_OK | MB_ICONWARNING);
					SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "");

					// Limpiar selección del combo
					HWND hCombo = GetDlgItem(hwnd, COMBO_MEDICOS);
					SendMessage(hCombo, CB_SETCURSEL, -1, 0);
				}
				

				modificar = true;
				EnableWindow(GetDlgItem(hwnd, IDC_ID_CONSULTORIO), FALSE);
				//MessageBox(hwnd, "Consultorio encontrado. Puedes modificarlo.", "Encontrado", MB_OK | MB_ICONINFORMATION);
			}
			else {
				MessageBox(hwnd, "Consultorio no encontrado.", "Error", MB_OK | MB_ICONERROR);
			}
		} break;

								   // ===== LIMPIAR FORMULARIO =====
		case IDC_LIMPAR_CONSULTORIO: {
			EnableWindow(GetDlgItem(hwnd, IDC_ID_CONSULTORIO), TRUE);
			SetDlgItemTextA(hwnd, IDC_ID_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, BUSCAR_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "");

			// Limpiar selección del combo
			HWND hCombo = GetDlgItem(hwnd, COMBO_MEDICOS);
			SendMessage(hCombo, CB_SETCURSEL, -1, 0);

			modificar = false;
			consultorioSeleccionado = NULL;
			llenarComboMedicos(hwnd); // Asegurarse de que el combo de médicos esté lleno después de limpiar
			//MessageBox(hwnd, "Formulario limpiado.", "Limpiar", MB_OK | MB_ICONINFORMATION);
		} break;

								   // ===== APLICAR (CREAR O MODIFICAR) =====
		case IDC_APLICAR_CONSULTORIO: {
			HWND hIdConsultorio = GetDlgItem(hwnd, IDC_ID_CONSULTORIO);

			int idConsultorio = 0;
			char buffer[20];
			GetWindowTextA(hIdConsultorio, buffer, 20);

			// Validaciones básicas
			if (strlen(buffer) == 0 && !modificar) {
				MessageBox(hwnd, "El ID del consultorio es obligatorio.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			if (sscanf(buffer, "%d", &idConsultorio) != 1 && !modificar) {
				MessageBox(hwnd, "ID inválido. Debe ser un número.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Obtener médico seleccionado
			int cedulaMedico = obtenerCedulaDelCombo(hwnd);
			if (cedulaMedico == -1) {
				MessageBox(hwnd, "Debe seleccionar un médico.", "Error", MB_OK | MB_ICONERROR);
				break;
			}

			// Validar que el médico no tenga ya un consultorio asignado (relación 1:1)
			/*if (!modificar) {
				
			}*/
			Consultorio* actual = primeroConsultorio;
			while (actual != NULL) {
				if (actual->cedulaMedico == cedulaMedico) {
					MessageBox(hwnd, "Este médico ya tiene un consultorio asignado.", "Error", MB_OK | MB_ICONERROR);
					return FALSE;
				}
				actual = actual->siguiente;
			}

			if (consultorioSeleccionado != NULL) {
				// MODIFICAR consultorio existente
				consultorioSeleccionado->cedulaMedico = cedulaMedico;
				consultorioSeleccionado->estatus = DISPONIBLE; // Estado por defecto

				MessageBox(hwnd, "Consultorio modificado correctamente.", "Modificado", MB_OK | MB_ICONINFORMATION);
				llenarComboMedicos(hwnd); // Asegurarse de que el combo de médicos esté lleno después de modificar
			}
			else {
				// CREAR nuevo consultorio

				// Validar que no exista ese ID
				if (buscarConsultorio(primeroConsultorio, idConsultorio)) {
					MessageBox(hwnd, "Ya existe un consultorio con este ID.", "Error", MB_OK | MB_ICONERROR);
					break;
				}

				// Crear y agregar nuevo consultorio
				Consultorio* nuevo = crearConsultorio(idConsultorio, DISPONIBLE);
				nuevo->cedulaMedico = cedulaMedico;
				agregarConsultorio(nuevo);

				MessageBox(hwnd, "Consultorio agregado correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
			}

			// Limpiar formulario después de aplicar
			EnableWindow(GetDlgItem(hwnd, IDC_ID_CONSULTORIO), TRUE);
			SetDlgItemTextA(hwnd, IDC_ID_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, BUSCAR_CONSULTORIO, "");
			SetDlgItemTextA(hwnd, IDC_ESPECIALIDAD_CONSULTORIO, "");
			SendMessage(GetDlgItem(hwnd, COMBO_MEDICOS), CB_SETCURSEL, -1, 0);

			modificar = false;
			consultorioSeleccionado = NULL;
			llenarListaConsultorios(hwnd);
		} break;
		}
	} break;

	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK ventanaReporteCitasMedico(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_COMMAND: {
		menu(hwnd, wParam);
		switch (LOWORD(wParam)) {
		/*case REGRESAR_BTN: {
			EndDialog(hwnd, 0);
			DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_MAIN_WINDOW), NULL, vInicio);
		} break;*/
		}
	} break;
	case WM_CLOSE: {
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}


LRESULT CALLBACK ventanaReporteCitasPaciente(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_COMMAND: {
		menu(hwnd, wParam);
		switch (LOWORD(wParam)) {
			/*case REGRESAR_BTN: {
				EndDialog(hwnd, 0);
				DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_MAIN_WINDOW), NULL, vInicio);
			} break;*/
		}
	} break;
	case WM_CLOSE: {
		guardarArchivosBinarios();
		PostQuitMessage(0);
	} break;
	}
	return FALSE;
}

void menu(HWND hwnd, WPARAM wParam) {
	switch (LOWORD(wParam)) {
	case ID_MEDICO:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_MEDICO), NULL, vDoctor);
	} break;
	case ID_PACIENTE:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_PACIENTE), NULL, vPaciente);
	} break;
	case ID_ESPECIALIDAD:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_ESPECIALIDAD), NULL, ventanaEspecialidad);
	} break;
	case ID_CONSULTORIO:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_CONSULTORIO), NULL, vConsultorio);
	} break;
	case ID_REPORTEDECITASMEDICO:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_REPORTE_CITAS_MEDICO), NULL, ventanaReporteCitasMedico);
	} break;
	case ID_REPORTEDECITASPACIENTE:
	{
		EndDialog(hwnd, 0);
		DialogBox(hInstGlobal, MAKEINTRESOURCE(IDD_REPORTE_PACIENTE), NULL, ventanaReporteCitasPaciente);
	} break;
	}
}


char carpeta[MAX_PATH];
void InicializarArchivos() {
	char citas_[] = "\\Citas.dat";
	char pacientes_[] = "\\Pacientes.dat";
	char medicos_[] = "\\Medicos.dat";
	char especialidades_[] = "\\Especialidades.dat";
	char consultorio_[] = "\\Consultorios.dat";


	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, citas_);
	strcpy(archivoCitas, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, pacientes_);
	strcpy(archivoPacientes, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, medicos_);
	strcpy(archivoMedicos, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, especialidades_);
	strcpy(archivoEspecialidades, carpeta);

	strcpy(carpeta, "");
	GetCurrentDirectory(MAX_PATH, carpeta);
	strcat(carpeta, consultorio_);
	strcpy(archivoConsultorios, carpeta);
}

void leerArchivosBinarios()
{
	std::ifstream lecturaCitas(archivoCitas, std::ios::binary);
	std::ifstream lecturaPacientes(archivoPacientes, std::ios::binary);
	std::ifstream lecturaMedicos(archivoMedicos, std::ios::binary);
	std::ifstream lecturaEspecialidades(archivoEspecialidades, std::ios::binary);
	std::ifstream lecturaConsultorio(archivoConsultorios, std::ios::binary);

	if (lecturaCitas.is_open())
	{
		leerCitas(lecturaCitas);
		lecturaCitas.close();
	}
	else
		MessageBox(NULL, "Error de lectura en citas", "error", MB_ICONERROR | MB_OK);

	if (lecturaPacientes.is_open())
	{
		leerPacientes(lecturaPacientes);
		lecturaPacientes.close();
	}
	else
		MessageBox(NULL, "Error de lectura en pacientes", "error", MB_ICONERROR | MB_OK);

	if (lecturaMedicos.is_open())
	{
		leerMedicos(lecturaMedicos);
		lecturaMedicos.close();
	}
	else
		MessageBox(NULL, "Error de lectura en medicos", "error", MB_ICONERROR | MB_OK);

	if (lecturaEspecialidades.is_open())
	{
		leerEspecialidad(lecturaEspecialidades);
		lecturaEspecialidades.close();
	}
	else
		MessageBox(NULL, "Error de lectura en especialidades", "error", MB_ICONERROR | MB_OK);

	if (lecturaConsultorio.is_open())
	{
		leerConsultorios(lecturaConsultorio);
		lecturaConsultorio.close();
	}
	else
		MessageBox(NULL, "Error de lectura en consultorios", "error", MB_ICONERROR | MB_OK);
}

void guardarArchivosBinarios()
{
	std::ofstream escrituraCitas(archivoCitas, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraPacientes(archivoPacientes, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraMedicos(archivoMedicos, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraEspecialidades(archivoEspecialidades, std::ios::binary | std::ios::trunc);
	std::ofstream escrituraConsultorio(archivoConsultorios, std::ios::binary | std::ios::trunc);

	if (escrituraCitas.is_open())
	{
		escribirCitas(escrituraCitas);
		escrituraCitas.close();
	}
	else
		MessageBox(NULL, "Error de escritura en citas", "error", MB_ICONERROR | MB_OK);

	if (escrituraPacientes.is_open())
	{
		escribirPacientes(escrituraPacientes);
		escrituraPacientes.close();
	}
	else
		MessageBox(NULL, "Error de escritura en pacientes", "error", MB_ICONERROR | MB_OK);

	if (escrituraMedicos.is_open())
	{
		escribirMedicos(escrituraMedicos);
		escrituraMedicos.close();
	}
	else
		MessageBox(NULL, "Error de escritura en medicos", "error", MB_ICONERROR | MB_OK);

	if (escrituraEspecialidades.is_open())
	{
		escribirEspecialidad(raiz, escrituraEspecialidades);
		escrituraEspecialidades.close();
	}
	else
		MessageBox(NULL, "Error de escritura en especialidades", "error", MB_ICONERROR | MB_OK);

	if (escrituraConsultorio.is_open())
	{
		escribirConsultorios(escrituraConsultorio);
		escrituraConsultorio.close();
	}
	else
		MessageBox(NULL, "Error de escritura en consultorios", "error", MB_ICONERROR | MB_OK);
}
