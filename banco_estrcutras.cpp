#include <iostream>
#include <windows.h>
#include <iomanip>
#include <sstream>
#include <time.h>
#include <string>
#include <vector>
#include <stack>
#include <queue> 
using namespace std;

int main();
struct node;




// === ESTRUCTURA 1: NODO DE CUENTA ===
struct node {
    string Name;
    string CNIC;
    char Gender;
    string Type;
    int Acc_Number;
    int pin;
    int bala;
    node *right, *left;
    int height;
};

// Variables globales del árbol
struct node *temp, *temp1, *temp2, *tempcell = NULL;


// === ESTRUCTURA 2: ARREGLO DINÁMICO DE TRANSACCIONES ===
struct Transaccion {
    int cuenta;
    string tipo;
    long long monto;
    string fecha;
};

// Vector global dinámico
vector<Transaccion> historialTransacciones;


// === ESTRUCTURA 3: PILA PARA DESHACER OPERACIONES ===
struct UndoOperacion {
    int cuenta;
    string tipo;
    long long monto;
};
stack<UndoOperacion> pilaUndo;


// === ESTRUCTURA 4: COLA DE QUEJAS ===
struct Queja {
    int cuenta;
    string descripcion;
    string fecha;
};

queue<Queja> colaQuejas;

// === FUNCIONES PARA LA COLA DE QUEJAS ===
void registrarQueja() {
    Queja q;
    cout << "\nIngrese el número de cuenta del cliente: ";
    cin >> q.cuenta;
    cin.ignore();
    cout << "Describa la queja o inconveniente: ";
    getline(cin, q.descripcion);

    time_t now = time(0);
    q.fecha = ctime(&now);

    colaQuejas.push(q);
    cout << "\n La queja ha sido registrada exitosamente.\n";
}

void verQuejas() {
    if (colaQuejas.empty()) {
        cout << "\nNo hay quejas registradas actualmente.\n";
        return;
    }

    queue<Queja> temp = colaQuejas;
    cout << "\n=============== LISTA DE QUEJAS (ORDEN DE LLEGADA) ===============\n";
    while (!temp.empty()) {
        Queja q = temp.front();
        temp.pop();
        cout << "Cuenta: " << q.cuenta << "\n";
        cout << "Fecha: " << q.fecha;
        cout << "Descripción: " << q.descripcion << "\n";
        cout << "---------------------------------------------------------------\n";
    }
}


// === FUNCIONES DE TRANSACCIONES ===
void agregarTransaccion(int cuenta, const string& tipo, long long monto) {
    time_t now = time(0);
    string fecha = ctime(&now);
    Transaccion t = {cuenta, tipo, monto, fecha};
    historialTransacciones.push_back(t);
    if (historialTransacciones.size() > 50)
        historialTransacciones.erase(historialTransacciones.begin());
}

void guardarOperacion(int cuenta, const string& tipo, long long monto) {
    pilaUndo.push({cuenta, tipo, monto});
}


// === MOSTRAR HISTORIAL DE UNA CUENTA ===
void mostrarHistorial(struct node* root) {
    int cuenta, pin;
    cout << "Ingrese el numero de cuenta: ";
    cin >> cuenta;
    cout << "Ingrese el PIN: ";
    cin >> pin;

    // Buscar cuenta
    node* temp = root;
    bool encontrada = false;
    while (temp != NULL && !encontrada) {
        if (cuenta > temp->Acc_Number) temp = temp->right;
        else if (cuenta < temp->Acc_Number) temp = temp->left;
        else if (cuenta == temp->Acc_Number && pin == temp->pin)
            encontrada = true;
        else break;
    }

    if (!encontrada) {
        cout << "\nCuenta o PIN incorrectos.\n";
        return;
    }

    cout << "\n==============================================\n";
    cout << "     HISTORIAL DE TRANSACCIONES - CUENTA " << cuenta << "\n";
    cout << "==============================================\n";

    bool hayMovimientos = false;
    for (auto& t : historialTransacciones) {
        if (t.cuenta == cuenta) {
            hayMovimientos = true;
            cout << "Fecha: " << t.fecha;
            cout << "Tipo: " << t.tipo << " | Monto: $" << t.monto << "\n";
            cout << "----------------------------------------------\n";
        }
    }

    if (!hayMovimientos)
        cout << "No hay transacciones registradas para esta cuenta.\n";

    cout << "==============================================\n";

    string volver;
    cout << "\nPresiona cualquier tecla para regresar al PANEL ADMIN :)...! ";
    cin >> volver;
}



// === DESHACER ÚLTIMA OPERACIÓN ===
void deshacerUltimaOperacion(struct node* root) {
    int cuenta, pin;
    cout << "Ingrese el numero de cuenta: ";
    cin >> cuenta;
    cout << "Ingrese el PIN: ";
    cin >> pin;

    // Buscar cuenta
    node* temp = root;
    bool encontrada = false;
    while (temp != NULL && !encontrada) {
        if (cuenta > temp->Acc_Number) temp = temp->right;
        else if (cuenta < temp->Acc_Number) temp = temp->left;
        else if (cuenta == temp->Acc_Number && pin == temp->pin)
            encontrada = true;
        else break;
    }

    if (!encontrada) {
        cout << "\nCuenta o PIN incorrectos.\n";
        return;
    }

    // Buscar última transacción
    bool hayTransaccion = false;
    Transaccion ultima;
    for (auto it = historialTransacciones.rbegin(); it != historialTransacciones.rend(); ++it) {
        if (it->cuenta == cuenta) {
            ultima = *it;
            hayTransaccion = true;
            break;
        }
    }

    if (!hayTransaccion) {
        cout << "No hay transacciones para deshacer.\n";
        return;
    }

    cout << "\n=== ULTIMA TRANSACCIÓN ===\n";
    cout << "Fecha: " << ultima.fecha;
    cout << "Tipo: " << ultima.tipo << " | Monto: $" << ultima.monto << "\n";
    cout << "----------------------------------------------\n";
    cout << "¿Desea deshacer esta operación? (1 = Sí / 0 = No): ";
    int opcion;
    cin >> opcion;
    if (opcion != 1) return;

    // Deshacer según tipo
    if (ultima.tipo == "Deposito" || ultima.tipo == "deposito") {
        temp->bala -= ultima.monto;
        cout << "Deposito revertido.  Nuevo saldo: $" << temp->bala << "\n";
        agregarTransaccion(cuenta, "Deshacer deposito", ultima.monto);
        guardarOperacion(cuenta, "Deshacer deposito", ultima.monto);
    }
    else if (ultima.tipo == "Retiro" || ultima.tipo == "retiro") {
        temp->bala += ultima.monto;
        cout << "Retiro revertido.   Nuevo saldo: $" << temp->bala << "\n";
        agregarTransaccion(cuenta, "Deshacer retiro", ultima.monto);
        guardarOperacion(cuenta, "Deshacer retiro", ultima.monto);
    }
    else {
        cout << "No se puede deshacer este tipo de operacion.\n";
    }

    string volver;
    cout << "\nPresiona cualquier tecla para regresar al PANEL ADMIN :)...!";
    cin >> volver;
}
/////////////////////////////////////////////////////////////////////////////////////////Main Functions used in AVL
// Max Function
int maximo(int a, int b){
    if(a > b){return a;}
    else{return b;}
}
// Height Function
int height(struct node* temp){
  if(temp == NULL){return 0;}
  int lh = height(temp->left);
  int rh = height(temp->right);
  if(lh > rh){return lh+1;}
  else{return rh+1;}
}
// Left Rotation Function
struct node *left_rotation(struct node* temp2) {
  temp1 = temp2->right;
  temp2->right = temp1->left;
  temp1->left = temp2;

  temp2->height = max(height(temp2->left), height(temp2->right)) + 1;
  temp1->height = max(height(temp1->left), height(temp1->right)) + 1;
  return temp1;
}
// Right Rotation Function
struct node *right_rotation(struct node* temp2){
  temp1 = temp2->left;
  temp2->left = temp1->right;
  temp1->right = temp2;

  temp2->height = max(height(temp2->left), height(temp2->right)) +1;
  temp1->height = max(height(temp1->left), height(temp1->right)) +1;

  return temp1;
}
// right left rotation
struct node *right_left_rotation(struct node* temp3)   {
  temp3->right = right_rotation(temp3->right);
  return left_rotation(temp3);
}
// Left right rotation
struct node *left_right_rotation(node *temp3)     {
  temp3->left = left_rotation(temp3->left);
  return right_rotation(temp3);
}
//Minimum Function{
struct node *minNode(struct node* temp) {
    if(temp == NULL){return 0;}
    if(temp->left == NULL){return temp;}
    else{return minNode(temp->left);}
}
// balance function
struct node *balance(struct node* temp)  {
 if(temp == NULL){cout<<"No rotation is found "<<endl;}
 if( height(temp->left)-height(temp->right) > 1){
 if(height(temp->left->left) >= height(temp->left->right)){temp =right_rotation(temp);}
 else{temp =left_right_rotation(temp);}}
 else if( height(temp->right)-height(temp->left) > 1){if(height(temp->right->right) >= height(temp->right->left))
 {
  temp =left_rotation(temp);
 }
 else
 {
  temp =right_left_rotation(temp);
 }
 }
  temp->height = max(height(temp->left), height(temp->right)) + 1;
  return temp;
}
/////////////////////////////////////////////////////////////////////////////////////////END of main Functions in AVL


struct node *insertion(struct node* temp,string name,string cnc,char gen,string type,int account_no,int PIN,int bal){    ///////////////////////// insertion
 if(temp == NULL){
  temp = new node();
  temp->Name          = name;
  temp->CNIC          = cnc;
  temp->Gender        = gen;
  temp->Type          = type;
  temp->Acc_Number    = account_no;
  temp->pin           = PIN;
  temp->bala          = bal;
  temp->left          = NULL;
  temp->right         = NULL;
  temp->height        = 0;}

if(account_no < temp->Acc_Number){
     temp->left = insertion(temp->left, name, cnc,gen,type, account_no,PIN,bal);
  if(height(temp->left) - height(temp->right) == 2){
     if(account_no < temp->left->Acc_Number){temp = right_rotation(temp);}
     else{temp = left_right_rotation(temp);}}}

if(account_no > temp->Acc_Number){
    temp->right = insertion(temp->right, name, cnc,gen,type, account_no,PIN,bal);
  if(height(temp->right) - height(temp->left) == 2){
    if(account_no > temp->right->Acc_Number){temp = left_rotation(temp);}
    else{temp = right_left_rotation(temp);}}}

 temp->height = max(height(temp->left), height(temp->right)) +1 ;
 return temp;
}


struct node *deletion(struct node* temp, int x) ///////////////////////// deletion
{
 if(temp == NULL){
		return temp;}
 else if(x < temp->Acc_Number){
		temp->left = deletion(temp->left, x);}
 else if(x > temp->Acc_Number){
		temp->right = deletion(temp->right, x);}
 else if(temp->left && temp->right){
		tempcell = minNode(temp->right);
		temp->Acc_Number = tempcell->Acc_Number;
		temp->right = deletion(temp->right, temp->Acc_Number);}
 else{
		tempcell = temp;
 if(temp->left == NULL){
		temp = temp->right;}
 else if(temp->right == NULL){
		temp = temp->left;}
		delete tempcell;}
 if(temp != NULL){
		temp = balance(temp);}

 return temp;

}

int findMax(struct node* root)  ///////////////////////// find max
{
 if (root == NULL){return 0;}
 int res = root->Acc_Number;
 int lres = findMax(root->left);
 int rres = findMax(root->right);
 if (lres > res){res = lres;}
 if (rres > res)
      res = rres;
    return res;
}

bool search(struct node* temp, int x)  ///////////////////////// search
{
  while (temp != NULL)
    {
      
        if (x> temp->Acc_Number)
            temp = temp->right;

        
        else if (x < temp->Acc_Number)
           temp = temp->left;
        else
            return true;
    }
    return false;

}


int print_data(struct node* temp, int x) ///////////////////////// Print Function
{
 while (temp != NULL)
    {
    // pass right subtree as new tree
    if (x> temp->Acc_Number)
        temp = temp->right;

    // pass left subtree as new tree
    else if (x < temp->Acc_Number)
        temp = temp->left;
    else{
        cout<<"        ####################################################################################################################################"<<endl;
        cout<<" "<<endl;
        cout<<"          "<<"Numero_C"<<"                 "<<"NOMBRE"<<"                   "<<"CC"<<"                     "<<"GENERO"<<"            "<<"Tipo"<<"          "<<"Saldo"<<endl;
        cout<<" "<<endl;
        cout<<"        ####################################################################################################################################"<<endl;
        cout<<" "<<endl;
        cout<<"             ";
        cout<<temp->Acc_Number;
        cout<<"            ";
        cout<<setw(18)<<temp->Name;
        cout<<"           ";
        cout<<temp->CNIC;
        cout<<"                ";
        cout<<temp->Gender;
        cout<<"                ";
        cout<<temp->Type;
        cout<<"             ";
        cout<<temp->bala<<endl;
        return true;
        }
    }
    return false;


}

void pre_order(struct node* temp)   ///////////////////////// pre order function
{
//cout << temp->pin<<endl;
if(temp->left != NULL){pre_order(temp->left);}
 //cout<<"          "<< setw(2)<<temp->Acc_Number;
 cout<<"             ";
 cout<<temp->Acc_Number;
 cout<<"            ";
 cout<<setw(18)<<temp->Name;
 cout<<"           ";
 cout<<temp->CNIC;
 cout<<"                ";
 cout<<temp->Gender;
 cout<<"                ";
 cout<<temp->Type;
 cout<<"             ";
 cout<<temp->bala<<endl;
 if(temp->right != NULL){pre_order(temp->right);}

}

void print_for_admin(struct node* temp)   ////////////////////////// print For order function
{
 if(temp->left != NULL){print_for_admin(temp->left);}
 cout<<"             ";
 cout<<temp->Acc_Number;
 cout<<"            ";
 cout<<setw(18)<<temp->Name;
 cout<<"           ";
 cout<<temp->CNIC;
 cout<<"                ";
 cout<<temp->Gender;
 cout<<"                ";
 cout<<temp->Type;
 cout<<"         ";
 cout<<temp->pin;
 cout<<"          ";
 cout<<temp->bala<<endl;
 if(temp->right != NULL){print_for_admin(temp->right);}

}



int deposit(struct node* temp,int acc_num,int PIN,int bal)
{
    while (temp != NULL) {
        if (acc_num > temp->Acc_Number) { temp = temp->right; }
        else if (acc_num < temp->Acc_Number) { temp = temp->left; }
        else {
            if (temp->pin == PIN) {
                temp->bala = temp->bala + bal;

                // === NUEVO: registrar transacción y guardar operación para deshacer ===
                agregarTransaccion(acc_num, "deposito", bal);
                guardarOperacion(acc_num, "deposito", bal);
            }
            else if (temp->pin != PIN) { return -1; }
            return 1;
        }
    }
    return 0;
}

int with_draw(struct node* temp,int acc_num,int PIN,int bal)   /////////////////////////  with_draw balance
{
 while (temp != NULL){
  if (acc_num> temp->Acc_Number){temp = temp->right;}
  else if (acc_num < temp->Acc_Number){temp = temp->left;}
  else{
    if(temp->pin == PIN){
        if((temp->bala < bal)){	return -1;	}
        else {temp->bala = temp->bala - bal;}}
	else if(temp->pin != PIN){return -2	;}
        return 1;}}
  return 0;

}


int change_info(struct node* temp,string name,string cnic,char gender,string type,int acc_num,int PIN)   ///////////////////////// Change Info
{

 while (temp != NULL){
  if (acc_num> temp->Acc_Number){temp = temp->right;}
  else if (acc_num < temp->Acc_Number){temp = temp->left;}
  else{
    if(temp->Acc_Number == acc_num){
           temp->Name = name;
           temp->CNIC = cnic;
           temp->Gender = gender;
           temp->pin = PIN;
           temp->Type = type;}

       return 1;}}
  return 0;

}

int check_acc_pin(struct node* temp,int acc_num,int PIN)   ///////////////////////// check acc & PIn number
{
 while (temp != NULL){
  if (acc_num> temp->Acc_Number){temp = temp->right;}
  else if (acc_num < temp->Acc_Number){temp = temp->left;}
  else{
    if(temp->pin == PIN){return 1;}
    else if(temp->pin != PIN){return -1;}}}

  return 0;

}


                                                          //-------------------xxxxxxxxxxxxxxxxx---------------------------\\


//Linked List Start
struct log{
       string LOG;
       log* down;

};
struct log_handler{
       int Account_Number;
       log_handler* next;
       log* down;
};
struct log_handler *head  = NULL;


void insertion_for_log(int account_number) //Inserion in Linked List
{
  struct log_handler *newnode = new log_handler;
  log_handler *temp=NULL;
  log_handler *temp2 = NULL;
  if(head == NULL){
     newnode -> Account_Number = account_number;
     newnode -> next = NULL;
     newnode -> down = NULL;
     head = newnode;}
  else{for(temp2 = head; temp2-> next != NULL ; temp2 = temp2 -> next){}
     temp =new log_handler;
     temp -> Account_Number = account_number;
     temp2 -> next = temp;
     temp ->  next = NULL;
     temp -> down = NULL;}
}

void insert_log(int number,string nn) //Insert Logs 
{
  {
  struct log_handler *temp2 = new log_handler;
  struct log *temp =new log;
  struct log *newnode =new log;
  newnode -> LOG  = nn;
  newnode -> down=NULL;
  for(temp2 = head ; temp2 != NULL; temp2 = temp2 -> next){
     if(temp2 -> Account_Number == number){
      if(temp2->down == NULL){
         temp2 -> down  = newnode;
         newnode -> down = NULL;}
       else{
         temp = temp2 -> down;
         temp2 -> down  = newnode;
         newnode -> down = temp;}}}
}
}

int  display(int acc_num)    //Display Function 
{                                
   struct log_handler *temp=new log_handler;
   struct log *temp2 = new log;
   if(head == NULL){return 0;}
   else{
      for(temp = head; temp != NULL; temp = temp -> next){
       if(temp-> Account_Number == acc_num){
        for(temp2 = temp->down; temp2 != NULL; temp2 = temp2-> down)
        {
      	cout<<"                                 -> " <<  temp2-> LOG <<endl;
		}
		break;
	return 1;

}}
}}
int  display1(int acc_num)    //Display Function 
{                                
   struct log_handler *temp=new log_handler;
   struct log *temp2 = new log;
   if(head == NULL){return 0;}
   else{
      for(temp = head; temp != NULL; temp = temp -> next){
       if(temp-> Account_Number == acc_num){
        for(temp2 = temp->down; temp2 != NULL; temp2 = temp2-> down)
        {
      	 temp2-> LOG;
		}
		
	return 1;

}}
}}

                                                    //-------------------xxxxxxxxxxxxxxxxx---------------------------\\


struct node *root = NULL;
int sleep_value =130;
int admin_pswd= 1122;
int staff_pswd= 2233;

int create_Acc()
{
  //Declared Variables
  string full_name;
  string cnic;
  string Acc_type;
  char g;
  int acc_no;
  int pin;
  long long Balance;
  int load=0;
  int a=1;
  //END Declared Variables
  cout<<"\n";
  cout<<"\n";
  cout<<"\n";
  cout<<"\n";
  cout<<""<<endl;
  cout<<"                                                 Crear cuenta de usuario"<<endl;
  cout<<" "<<endl;
  cout<<"                                            Ingrese el nombre completo del cliente: ";cin.ignore();getline(cin,full_name);
  cout<<" "<<endl;
  cout<<"                                            Ingrese el número de CC  (xxxxx-xxxxxxx-x) : ";getline(cin,cnic);
  cout<<" "<<endl;
  cout<<"                                            Ingrese el genero (M/F) : ";cin>>g;
  cout<<" "<<endl;
  cout<<"                                            Ingrese el tipo de cuenta (C/A) : ";cin.ignore();cin>>Acc_type;
  cout<<" "<<endl;
  load=findMax(root);
  acc_no = load + 1;
  cout<<"                                            Su número de cuenta es : "<<acc_no<<endl;
  cout<<" "<<endl;
  cout<<"                                            Ingrese su Pin : " ;cin.ignore();cin>>pin;
  cout<<" "<<endl;
  jump2:
  cout<<"                                            Ingrese el saldo inicial (minimo: 50 COP): ";cin.ignore();cin>> Balance;
  if(Balance >= 50){goto jump3;}
  else{cout<<"                                            El saldo minimo es de 50 COP. "<<endl;goto jump2;}

  jump3:
      root = insertion(root,full_name,cnic,g,Acc_type,acc_no,pin,Balance); //insertion(root)
      insertion_for_log(acc_no);
  return 1;
}


int admin_panel()
{
  //Declared Variables
  int b;
  int admin_delete_cin;
  int admin_search_cin;
  int result_admin_1;
  int result_admin_2;
  int result_admin_3;
  int result_admin_4;
  int admin_return;

  int acc_1;
  int pin_1;
  string cnc_1;
  char gen_1;
  string acc_type_1;
  string new_cnc_1;
  int new_pin_1;
  string new_acc_1;
  int admin_logs_cin;
  //END Declared Variables

  /////////////JUMP_2
  jump2:
  system("Color B");
  system("cls");
  cout<<"\n";
  cout<<"\n";
  cout<<"\n";
  cout<<"\n";
  cout<<"                                                       ********************************************************"<<endl;
  cout<<"                                                       ***********************PANEL |||ADMIN*******************"<<endl;
  cout<<"                                                       ********************************************************"<<endl;
  cout<<"                                                       *  Presione 1 para crear una cuenta                    *"<<endl;
  cout<<"                                                       *  Presione 2 para eliminar una cuenta                 *"<<endl;
  cout<<"                                                       *  Presione 3 para buscar una cuenta                   *"<<endl;
  cout<<"                                                       *  Presione 4 para ver todas las cuentas               *"<<endl;
  cout<<"                                                       *  Presione 5 para editar informacion                  *"<<endl;
  cout<<"                                                       *  Presione 6 para ver registros de cuentas eliminadas *"<<endl;
  cout<<"                                                       *  Presione 7 para volver al menu principal            *"<<endl;
  cout<<"                                                       *  Presione 8 para ver transacciones recientes         *"<<endl;   // === NUEVO
  cout<<"                                                       *  Presione 9 para deshacer la ultima operacion        *"<<endl;   // === NUEVO
  cout<<"                                                       *  Presione 10 para ver quejas registradas             *"<<endl;   // === NUEVO
  cout<<"                                                       *******************************************************"<<endl;
  cout<<"                                                       Ingrese una opcion: ";
  cin>>b;
if(b == 1){
    lol_admin_1:
    system("cls");
    create_Acc();
    string volver;
    cout << "\nPresione 1 para crear otra cuenta o cualquier tecla para regresar al PANEL DE ADMIN... ";
    cin >> volver;
    if(volver == "1"){ goto lol_admin_1; }
    else { goto jump2; }
}

else if(b == 2){
    lol_admin_3:
    system("cls");
    cout<<"                                         Ingrese el numero de cuenta a eliminar:";
    cin>>admin_delete_cin;
    result_admin_1 = search(root,admin_delete_cin);
    cout<<result_admin_1<<endl;
    if(result_admin_1 == 1){
        cout<<"                                         Numero de cuenta encontrado....!!!"<<endl;
        cout<<" " <<endl;
        deletion(root,admin_delete_cin);
        cout<<" " <<endl;
        cout<<"                                         La cuenta ha sido eliminada exitosamente....!!!"<<endl;
    }
    else{
        cout<<" " <<endl;
        cout<<"                                         Cuenta no encontrada ....!!!"<<endl;
        cout<<" "<<endl;
    }
    string volver;
    cout << "\nPresione 2 para eliminar otra cuenta o cualquier tecla para regresar al PANEL ADMIN... ";
    cin >> volver;
    if(volver == "2"){ goto lol_admin_3; }
    else { goto jump2; }
}

else if(b == 3){
    lol_admin_5:
    system("cls");
    cout<<"                                         Ingrese el numero de cuenta a buscar : ";
    cin>>admin_search_cin;
    result_admin_2 = search(root,admin_search_cin);
    if(result_admin_2 == 1){
        cout<<" "<<endl;
        cout<<"                                         Usuario encontrado..!!"<<endl;
        cout<<" "<<endl;
        print_data(root,admin_search_cin);
        cout<<" "<<endl;
    }
    else {
        cout<<" "<<endl;
        cout<<"                                         Usuario no encontrado..!! :("<<endl;
        cout<<" "<<endl;
    }
    string volver;
    cout << "\nPresione 3 para buscar otra cuenta o cualquier tecla para regresar al PANEL ADMIN... ";
    cin >> volver;
    if(volver == "3"){ goto lol_admin_5; }
    else { goto jump2; }
}

else if(b == 4){
    lol_admin_7:
    system("cls");
    cout<<"        ####################################################################################################################################"<<endl;
    cout<<" "<<endl;
    cout<<"          "<<"Numero_C"<<"                 "<<"NOMBRE"<<"                   "<<"CC"<<"                     "<<"GENERO"<<"            "<<"Tipo"<<"          "<<"PIN"<<"          "<<"SALDO"<<endl;
    cout<<" "<<endl;
    cout<<"        ####################################################################################################################################"<<endl;
    cout<<" "<<endl;
    print_for_admin(root);
    string volver;
    cout << "\nPresione 4 para ver la lista de cuentas otra vez o cualquier tecla para regresar al PANEL ADMIN... ";
    cin >> volver;
    if(volver == "4"){ goto lol_admin_7; }
    else { goto jump2; }
}

else if(b == 5){
    lol_admin_9:
    system("cls");
    cout<<"                                            Modificar informacion de una cuenta "<<endl;
    cout<<"                                            Ingrese el numero de cuenta del usuario: ";
    cin>>acc_1;
    cout<<"                                            Ingrese el PIN del usuario: ";
    cin>>pin_1;
    result_admin_3 = check_acc_pin(root,acc_1,pin_1);
    if(result_admin_3 == 1){
        cout<<"                                            Modificar informacion de la cuenta "<<endl;
        cout<<"\n"<<endl;
        cout<<"                                            Ingrese el nuevo nombre / o el anterior: ";
        cin>>new_acc_1;
        getline(cin, new_acc_1);
        cout<<"                                            Ingrese el nuevo numero de CC /o el anterior : ";
        getline(cin, new_cnc_1);
        cout<<"                                            Ingrese el genero : ";
        cin>>gen_1;
        cout<<"                                            Ingrese el tipo de cuenta C/A : ";
        cin>> acc_type_1;
        cout<<"                                            Ingrese el nuevo PIN /o el anterior : ";
        cin>> new_pin_1;
        change_info(root,new_acc_1,new_cnc_1,gen_1,acc_type_1,acc_1,new_pin_1);
        cout<<"                                            La informacion de la cuenta se ha modificado correctamente"<<endl;
    }
    else if(result_admin_3 == -1){ cout<<"                                            El PIN es incorrecto...!!!"<<endl; }
    else { cout<<"                                            La cuenta no existe...!!!"<<endl; }

    string volver;
    cout << "\nPresione 5 para modificar otra cuenta o cualquier tecla para regresar al PANEL ADMIN... ";
    cin >> volver;
    if(volver == "5"){ goto lol_admin_9; }
    else { goto jump2; }
}

else if(b == 6){
    lol_admin_11:
    system("cls");
    cout<<"                                         Ingrese el numero de cuenta: ";
    cin>>admin_logs_cin;
    result_admin_4 = display1(admin_logs_cin);
    print_data(root,admin_logs_cin);
    display(admin_logs_cin);

    string volver;
    cout << "\nPresione 6 para consultar otra cuenta o cualquier tecla para regresar al PANEL ADMIN... ";
    cin >> volver;
    if(volver == "6"){ goto lol_admin_11; }
    else { goto jump2; }
}

else if(b == 7) {
    system("cls");
    main();   
}

else if (b == 8) {
    system("cls");
    mostrarHistorial(root);
    goto jump2;
}

else if (b == 9) {
    system("cls");
    deshacerUltimaOperacion(root);
    goto jump2;
}

else if(b == 10){
    system("cls");
    cout << "=====================================================================\n";
    cout << "                     CONSOLA DE QUEJAS REGISTRADAS  \n";
    cout << "=====================================================================\n";
    cout << "  Aquí podrás ver todas las quejas registradas por los clientes.\n";
    cout << "  Cada queja aparece en orden de llegada (primero en entrar, primero en salir).\n";
    cout << "---------------------------------------------------------------------\n";

    if (colaQuejas.empty()) {
        cout << "\n  No hay quejas registradas actualmente.\n";
    } else {
        queue<Queja> temp = colaQuejas;
        int contador = 1;
        while (!temp.empty()) {
            Queja q = temp.front();
            temp.pop();

            cout << "\n[" << contador++ << "] ---------------------------------------------------\n";
            cout << " Cuenta: " << q.cuenta << "\n";
            cout << " Fecha: " << q.fecha;
            cout << "  Descripción: " << q.descripcion << "\n";
            cout << "---------------------------------------------------------------\n";
        }
    }

    cout << "=====================================================================\n";
    string volver;
    cout << "\nPresione cualquier tecla para regresar al PANEL ADMIN... ";
    cin >> volver;
    goto jump2;
}

return 0;
}


int staff_panel(){
   //Variables
   int staff_acc_no, staff_pin_no, staff_amount_to_dep;
   int staff_acc_no_1, staff_pin_no_1, staff_amount_to_wdraw_1;
   int account_logs_cin, account_info_cin;
   int c, result_staff_1, result_staff_2, result_staff_3, result_staff_4;
   string volver;

   jump3:
   system("Color B");
   system("cls");
   cout<<"\n\n\n\n";
   cout<<"                                                       ********************************************************"<<endl;
   cout<<"                                                       ***********************PANEL|||PERSONAL*****************"<<endl;
   cout<<"                                                       ********************************************************"<<endl;
   cout<<"                                                       *  Presione 1 para consultar informacion de cuenta     *"<<endl;
   cout<<"                                                       *  Presione 2 para depositar dinero                    *"<<endl;
   cout<<"                                                       *  Presione 3 para consultar los registros del usuario *"<<endl;
      cout<<"                                                    *  Presione 4 para registrar una queja de cliente      *"<<endl;
   cout<<"                                                       *  Presione 5 para volver al Menu Principal            *"<<endl;
   cout<<"                                                       ********************************************************"<<endl;
   cout<<"                                                       Ingrese una opcion: ";
   cin >> c;

   if(c == 1){
        lol_staff_1:
        system("cls");
        cout<<"                                         Ingrese el numero de cuenta para consultar la informacion: ";
        cin >> account_info_cin;
        result_staff_1 = search(root, account_info_cin);

        if(result_staff_1 == 1){
            cout<<"\n                                         Usuario encontrado..!!\n"<<endl;
            print_data(root, account_info_cin);
        }
        else{
            cout<<"\n                                         Usuario no encontrado..!! :(\n"<<endl;
        }

        cout<<"\nPresione 1 para buscar otra cuenta o cualquier tecla para regresar al PANEL DEL PERSONAL: ";
        cin >> volver;
        if(volver == "1"){ goto lol_staff_1; }
        else { goto jump3; }
   }

   else if(c == 2){
        lol_staff_3:
        system("cls");
        cout<<"                                         Ingrese el numero de cuenta: ";
        cin >> staff_acc_no;
        cout<<"                                         Ingrese el PIN de la cuenta: ";
        cin >> staff_pin_no;
        cout<<"                                         Ingrese la cantidad a depositar: ";
        cin >> staff_amount_to_dep;

        cout<<"\n                                                   Estado ANTES del deposito en la cuenta\n"<<endl;
        print_data(root, staff_acc_no);

        result_staff_2 = deposit(root, staff_acc_no, staff_pin_no, staff_amount_to_dep);
        if(result_staff_2 == 1){
            cout<<"\n                                                   Estado DESPUES del depósito en la cuenta\n"<<endl;
            print_data(root, staff_acc_no);

            time_t now = time(0);
            char* dt = ctime(&now);
            ostringstream str1;
            str1 << staff_amount_to_dep;
            string add = "Cantidad " + str1.str() + " ha sido depositada el " + dt;
            cout<<"\n                                         Se ha depositado la cantidad de "<<staff_amount_to_dep<<" en la cuenta numero "<<staff_acc_no<<endl;
            insert_log(staff_acc_no, add);
            agregarTransaccion(staff_acc_no, "Deposito", staff_amount_to_dep);
            guardarOperacion(staff_acc_no, "Deposito", staff_amount_to_dep);
        }
        else if(result_staff_2 == 0){ cout<<"Cuenta no encontrada... ¡El dinero no fue depositado!"<<endl; }
        else if(result_staff_2 == -1){ cout<<"El PIN es incorrecto"<<endl; }

        cout<<"\nPresione 2 para depositar en otra cuenta o cualquier tecla para regresar al PANEL DEL PERSONAL: ";
        cin >> volver;
        if(volver == "2"){ goto lol_staff_3; }
        else { goto jump3; }
   }

   else if(c == 3){
        lol_staff_7:
        system("cls");
        cout<<"                                         Ingrese el numero de cuenta: ";
        cin >> account_logs_cin;
        result_staff_4 = display1(account_logs_cin);
        print_data(root, account_logs_cin);

        if(result_staff_4 == 1){ display(account_logs_cin); }
        else { cout<<"                                         No se encontraron registros....!!!"<<endl; }

        cout<<"\nPresione 4 para consultar otra cuenta o cualquier tecla para regresar al PANEL DEL PERSONAL: ";
        cin >> volver;
        if(volver == "4"){ goto lol_staff_7; }
        else { goto jump3; }
   }

  else if(c == 4){
    system("cls");
    cout << "=============================================================\n";
    cout << "                 REGISTRO DE QUEJAS DE CLIENTES\n";
    cout << "=============================================================\n";
    cout << "\nEstimado cliente, por favor cuéntenos su inconveniente.\n";
    cout << "Nuestro personal revisará su caso lo antes posible.\n";
    cout << "-------------------------------------------------------------\n";

    registrarQueja();

    string volver;
    cout << "\nPresione cualquier tecla para regresar al PANEL DEL PERSONAL: ";
    cin >> volver;
    goto jump3;
}

   else if(c == 5){
        system("cls");
        main();  
   }

   else{
        goto jump3;
   }

   return 0;
}

int atm_panel()
{
    // Declaración de variables
    int d;
    int atm_acc_no_1;
    int atm_pin_no_1;
    int atm_amount_to_wdraw_1;
    int result_atm_3;
    string volver;

    jump4:
    system("Color B");
    system("cls");
    cout<<"\n\n\n\n";
    cout<<"                                                       ******************************************"<<endl;
    cout<<"                                                       *******************CAJERO*****************"<<endl;
    cout<<"                                                       ******************************************"<<endl;
    cout<<"                                                       *  Presione 1 para Retirar Dinero        *"<<endl;
    cout<<"                                                       *  Presione 2 para Regresar al Menu      *"<<endl;
    cout<<"                                                       ******************************************"<<endl;
    cout<<"                                                       Ingrese una opcion : ";
    cin >> d;

    if(d == 1){
        lol_atm_1:
        system("cls");
        cout<<"                                         Ingrese el numero de cuenta : ";
        cin >> atm_acc_no_1;
        cout<<"                                         Ingrese el PIN de la cuenta : ";
        cin >> atm_pin_no_1;
        cout<<"                                         Ingrese la cantidad a retirar : ";
        cin >> atm_amount_to_wdraw_1;

        cout<<"\n                                                   Estado ANTES de retirar dinero de la cuenta\n"<<endl;
        print_data(root, atm_acc_no_1);

        result_atm_3 = with_draw(root, atm_acc_no_1, atm_pin_no_1, atm_amount_to_wdraw_1);

        if(result_atm_3 == 1){
            cout<<"\n                                                   Estado DESPUES de retirar dinero de la cuenta\n"<<endl;
            print_data(root, atm_acc_no_1);

            time_t now = time(0);
            char* dt = ctime(&now);
            ostringstream str1;
            str1 << atm_amount_to_wdraw_1;
            string add2 = "Cantidad " + str1.str() + " retirada el " + dt;
            cout<<"\n                                         Se ha retirado la cantidad de "<<atm_amount_to_wdraw_1<<" de la cuenta número "<<atm_acc_no_1<<endl;
            insert_log(atm_acc_no_1, add2);
            agregarTransaccion(atm_acc_no_1, "Retiro", atm_amount_to_wdraw_1);
            guardarOperacion(atm_acc_no_1, "Retiro", atm_amount_to_wdraw_1);
        }
        

        else if(result_atm_3 == 0){
            cout<<"Cuenta no encontrada... El dinero no fue retirado."<<endl;
        }
        else if(result_atm_3 == -1){
            cout<<"Saldo insuficiente en la cuenta."<<endl;
        }
        else if(result_atm_3 == -2){
            cout<<"El PIN es incorrecto."<<endl;
        }

        cout<<"\nPresione 1 para retirar de otra cuenta o cualquier tecla para regresar al PANEL DEL CAJERO: ";
        cin >> volver;
        if(volver == "1"){ goto lol_atm_1; }
        else { goto jump4; }
    }

    else if(d == 2){
        system("cls");
        main(); 
    }

    else {
        goto jump4;
    }

    return 0;
}

int main()
{
  //Declare RECORD
    root = insertion(root,"Jonatan Gomez Perdomo","12345-6789101-1",'M',"C",32001,1234,500);
    insertion_for_log(32001);
  //END of Declare RECORD

  //Declare Variables
  int a;
  int b;
  system("MODE 150, 40");
  //End Variable Declaration
  /////////////JUMP_1
  jump1:
  cout<<"\n";
  cout<<"\n";
  cout<<"\n";
  cout<<"\n";


  system("Color B");
  system("cls");
    cout << "\n";
    cout << "  ______      _                   _                 _                _             " << endl;
    cout << " |  ____|    | |                 | |               | |              | |            " << endl;
    cout << " | |__  _   _| |_ ___ _ __ _   _| |__   ___   ___ | | ___  _ __ ___| |_ ___  ___  " << endl;
    cout << " |  __|| | | | __/ _ \\ '__| | | | '_ \\ / _ \\ / _ \\| |/ _ \\| '__/ _ \\ __/ _ \\/ __| " << endl;
    cout << " | |___| |_| | ||  __/ |  | |_| | |_) | (_) | (_) | | (_) | | |  __/ ||  __/\\__ \\ " << endl;
    cout << " |______\\__,_|\\__\\___|_|   \\__, |_.__/ \\___/ \\___/|_|\\___/|_|  \\___|\\__\\___||___/ " << endl;
    cout << "                             __/ |                                               " << endl;
    cout << "                            |___/                                                " << endl;
    cout << "                                                                                 " << endl;
    cout << "               ____        _        _               _            " << endl;
    cout << "              |  _ \\  __ _| |_ __ _| |__   ___  ___| |_ ___ _ __ " << endl;
    cout << "              | | | |/ _` | __/ _` | '_ \\ / _ \\/ __| __/ _ \\ '__|" << endl;
    cout << "              | |_| | (_| | || (_| | |_) |  __/ (__| ||  __/ |   " << endl;
    cout << "              |____/ \\__,_|\\__\\__,_|_.__/ \\___|\\___|\\__\\___|_|   " << endl;
    cout << "                                                                  " << endl;
    cout << "\n";
    cout << "                                                                 Sistema de Gestion Bancaria                                                           " << endl;
    cout << "\n" << endl;
    cout << "                                                       ***********************************************" << endl;
    cout << "                                                       ******************LOGIN||PANEL*****************" << endl;
    cout << "                                                       ***********************************************" << endl;
    cout << "                                                       **  Presione 1 para ingresar como ADMIN      **" << endl;
    cout << "                                                       **  Presione 2 para ingresar como PERSONAL   **" << endl;
    cout << "                                                       **  Presione 3 para acceder al CAJERO        **" << endl;
    cout << "                                                       **  Presione 4 para SALIR                    **" << endl;
    cout << "                                                       ***********************************************" << endl;
    cout << "                                                       ***********************************************" << endl;
    cout << "\n" << endl;
    cout << "                                                       Ingrese su opcion: ";
    cin >> a;
    if (a == 1) {
        jump2:
        system("Color B");
        system("cls");
        cout<<"\n";
        cout<<"\n";
        cout<<"\n";
        cout<<"\n";
        cout<<"                                                       Ingrese Pin : ";
        cin >>b;
        if(b == admin_pswd){admin_panel();}
        else{goto jump2;}}
  else if(a == 2){
        jump3:
        system("Color B");
        system("cls");
        cout<<"\n";
        cout<<"\n";
        cout<<"\n";
        cout<<"\n";
        cout<<"                                                       Ingrese Pin : ";
        cin >>b;
        if(b == staff_pswd){staff_panel();}
        else{goto jump3;}}
  else if(a == 3){atm_panel();}
  else if(a == 4){exit(0);}
  else {goto jump1;}


}