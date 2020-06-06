"""
    FILE: error.py
    AUTHOR: Alba Ramos Pedroviejo - alba.ramosp@estudiante.uam.es
    AUTHOR: Nicolas Serrano Salas - nicolas.serranos@estudiante.uam.es
    DESCRIPTION: handles the errors returned by the SecureBox API
"""


"""
    METHOD: tratarError(ret)
    ARGS_IN: ret - what a call to the API returns
    DESCRIPTION: shows a different message depending on the error code returned by the API, and
                 gives instructions to the user to fix it.
    AGS_OUT: -
"""
def tratarError(ret):
    if ret['error_code'] == 'USER_ID1':
        print(str(ret['description']))
        print("Puede encontrar el ID de un usuario mediante --search_id")
    
    elif ret['error_code'] == 'USER_ID2':
        print(str(ret['description']))
        print("Pruebe introduciendo el nombre o el correo del usuario")
    
    elif ret['error_code'] == 'ARGS1':
        print(str(ret['description']))
    
    elif ret['error_code'] == 'TOK1':
        print(str(ret['description']))
        print("Puede consultar su token en el correo electronico que le fue enviado desde SecureBox, u obtener uno nuevo")
    
    elif ret['error_code'] == 'TOK2':
        print(str(ret['description']))
        print("Puede volver a obtener un token en la pagina de SecureBox")
    
    elif ret['error_code'] == 'TOK3':
        print(str(ret['description']))
        print("El formato de la cabecera es Authorization:Bearer token")
    
    elif ret['error_code'] == 'FILE1':
        print(str(ret['description']))
        print("Consulte el tamaño de su fichero mediante boton derecho-propiedades")
        
    elif ret['error_code'] == 'FILE2':
        print(str(ret['description']))
        print("Asegurese de que la ruta al fichero es correcta y tiene permisos")
        
    elif ret['error_code'] == 'FILE3':
        print(str(ret['description']))
        print("Borre algun fichero que ya no necesite")

    else:
        print("ERROR desconocido: "+str(ret))