// ignore_for_file: prefer_const_constructors

import 'dart:convert';
import 'dart:io' show Platform;
import 'dart:io';
import 'dart:convert';
import 'dart:typed_data';
import 'package:controle/hive_config.dart';
import 'package:flutter/material.dart';
import 'package:flutter_reactive_ble/flutter_reactive_ble.dart';
import 'package:hive_flutter/adapters.dart';
import 'package:loading_icon_button/loading_icon_button.dart';
import 'package:flutter_joystick/flutter_joystick.dart';
import 'package:esp_smartconfig/esp_smartconfig.dart';


void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await HiveConfig.start();
  runApp(const MainApp());
}

class MainApp extends StatelessWidget {
  const MainApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(
      home: MainPage(),
    );
  }
}

class MainPage extends StatefulWidget {
  const MainPage({super.key});
  @override
  State<StatefulWidget> createState() => MainPageState();
}

class MainPageState extends State<MainPage>with SingleTickerProviderStateMixin {

  //UI e UX
  late final TabController tabController = TabController(length: 3, vsync: this);
  final LoadingButtonController btnConnectController = LoadingButtonController();

  final TextEditingController mapa = TextEditingController();
  final TextEditingController serial = TextEditingController();
  final TextEditingController KI = TextEditingController();
  final TextEditingController KP = TextEditingController();
  final TextEditingController KD = TextEditingController();
  final TextEditingController VEL_MAX = TextEditingController();
  final TextEditingController VEL_MIN = TextEditingController();
  final TextEditingController TMP_FORA = TextEditingController();
  final TextEditingController MAPA = TextEditingController();
  final TextEditingController K = TextEditingController();
  final TextEditingController TOL = TextEditingController();

  Color corBotAzul = Color.fromARGB(255, 0, 255, 255);

  String tipoCnx = "BLE";
  List<String> FilaCMD = [];

  //Variaveis para o fucionamento do bluetooth serial
  final Uuid SERVICE_ID = Uuid.parse("4cc35f22-1978-41da-b944-ac9fdc39b747");
  final Uuid CHAR_E_ID =  Uuid.parse("016210bb-89a6-41af-9db9-2cef7bdf36eb");
  final Uuid CHAR_S_ID =  Uuid.parse("92da7b08-bc25-4bfd-b5ac-0e1c722310b1");
  final flutterReactiveBle = FlutterReactiveBle();
  late DiscoveredDevice bleRobo;
  List<DiscoveredDevice> dispsBLE = List<DiscoveredDevice>.empty(growable: true);
   List<String> entries = List.empty(growable: true);

  // Variaveis para WIFI
  RawDatagramSocket? socket;


  //Status
  bool initial = true;
  bool conectando = false;
  bool conectado = false;
  bool lendo = false;
  bool enviando = false;
  static bool escaneando = false;
  var listening;
  
  //Hive - Banco de dados
  static String actualBox = "";

  //Constantes recebidas do robo
  static String KI_robot = "";
  static String KP_robot = "";
  static String KD_robot = "";
  static String VEL_MAX_robot = "";
  static String VEL_MIN_robot = "";
  static String TMP_FORA_robot = "";
  static String K_robot = "";
  static String TOL_robot = "";
  static bool USE_MAP = false;

  //Joystick
  double _x = 0, _y = 0, step = 100;

  //Inicio App
  MainPageState(){
    if(!initial) setState((){});
    else{
      INIT_WIFI();
    }
    
    initial = false;
  }

  //wifi
  
  void INIT_WIFI() async{
    if(socket !=null) return;
    socket = await RawDatagramSocket.bind(InternetAddress.anyIPv4, 8889);
    socket?.broadcastEnabled = true;

    socket?.listen((RawSocketEvent event) {
      if (event == RawSocketEvent.read) {
        Datagram? datagram = socket?.receive();
        if (datagram != null) {
          String message = utf8.decode(datagram.data);
          // Handle the received message
          readWIFI(message);
        }
      }
    });
  }
  void mudarTipoCnx() async{
    if(tipoCnx != "BLE") tipoCnx = "BLE";
    else tipoCnx = "WIFI";
    setState(() {});
    
  }
  
  //Funcoes para interface e teste
  Future<bool> backAndroid() async{ //Altera o comportamento do comando "voltar" do android
    setState((){});
    
    if(conectando){
      conectando = false;
      btnConnectController.stop();
    }
    else if(tabController.index > 0) 
      {tabController.index -= 1;}
    
    return false;
  } 

  void showMsg(String text, BuildContext context){ // Exibe uma mensagem
    showDialog(context: context,builder: (context) {
        return AlertDialog(
          content: Text(text),
        );
      }
    );
  }

  //Widgets personalizados
  Future<Widget> listViewDisps() async{ //Lista de dispositivos encontrados - BLE
    return SizedBox(
      height: 300.0,
      width: 300.0,
      child: ListView.builder(
        
        scrollDirection: Axis.vertical,
        shrinkWrap: true,
        padding: const EdgeInsets.all(8),
        itemCount: entries.length,
        itemBuilder: (BuildContext context, int index) {
          return TextButton(
            onPressed: () => {conectar(index)},
            child: SizedBox(
              height: 50,
              child: Center(child: Text(entries[index], style: TextStyle(fontSize: 20, color: Colors.black),)),
            )
            
          );
      }),
    );
  }
  
  Container testButton(){ // Botao que envia comandos de teste
    void onchanged(dynamic value){
      if(value != "W") sendCMD(value);
      else mudarTipoCnx();
    }

    return 
      Container(
        padding: EdgeInsets.symmetric(horizontal: 20),
        decoration: BoxDecoration(borderRadius: BorderRadius.circular(20), color: Colors.black),
        child: DropdownButton(
          items: const [
            DropdownMenuItem(
              value: "F",
              child: Text("Teste Frontal"),
            ),
            DropdownMenuItem(
              value: "L",
              child: Text("Teste Lateral"),
            ),
            DropdownMenuItem(
              value: "T",
              child: Text("Recebe Const"),
            ),
            DropdownMenuItem(
              value: "W",
              child: Text("WIFI"),
            ),
          ],
          onChanged: onchanged,
          hint: Text("Testar", style: TextStyle(color: Colors.white),),
        )
      );
  }
  
  Expanded colunaMetadeTela(List<Widget> filhos){
    return
      Expanded(
        flex: 1,
        child: Padding(
          padding: EdgeInsets.fromLTRB(10, 20, 10, 20),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: filhos,
          )
        ),
      
      );
  }

  Container campoConst(String nome, TextEditingController ctrl, {String vlAtual = "", int flex = 1}){
    double sfont = 18;
    return Container(margin: EdgeInsets.only(bottom: 5), padding: EdgeInsets.all(8), decoration: BoxDecoration(borderRadius: BorderRadius.circular(10), color: Color.fromRGBO(0, 0, 0, 1)), child:
      Column( children:[
        Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Flexible(flex: 1, child: Text(nome, style: TextStyle(fontSize: sfont, color: Color.fromARGB(255, 255, 255, 255)),)) ,
            Expanded(
              flex: 2,
              child: TextField(
                    controller: ctrl,
                    style: TextStyle(fontSize: sfont, color: Color.fromARGB(255, 255, 255, 255)),
                    textAlign: TextAlign.center,
                    keyboardType: TextInputType.number,
                    decoration: const InputDecoration(
                      enabledBorder: OutlineInputBorder(borderSide: BorderSide(color: Color.fromARGB(255, 255, 255, 255))),
                    focusedBorder: OutlineInputBorder(borderSide: BorderSide(color: Color.fromARGB(255, 0, 85, 255)))
                    ),
                  )
            ),
            
            ]
        ),
        Container(
          decoration: BoxDecoration(borderRadius: BorderRadius.circular(30),color: corBotAzul,),
          margin: EdgeInsets.all(8),
          padding: EdgeInsets.all(3),
          
          width: double.infinity,
          child: Text("Atual: $vlAtual", style: TextStyle(fontSize: 18),),
        ),
      ])
    );
  }

  Expanded fillButton(Function onpressed, Widget child, double borderRadius){
    return Expanded( 
      child: ElevatedButton(
        onPressed: () => onpressed(),
        style: ElevatedButton.styleFrom(
          backgroundColor: Color.fromARGB(255, 0, 0, 0),
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(borderRadius)),
          minimumSize: const Size.fromHeight(60),
        ),
        child: child,
      ),
    );
  }
  
  void conectadoEvent(){
    conectado = true;
    conectando = false;
    listening.cancel();
    serial.text = "";
    sendCMD("T");
    btnConnectController.success();
    Future.delayed(Duration(milliseconds: 2500), (){btnConnectController.stop();});
  }
  void conectandoEvent(){
    conectando = true;
    conectado = false;
  }
  void desconectadoEvent(){
    conectando = false;
    conectado = false;
  }

  //Funcoes basicas do Bluetooth Serial 
  Future<void> conectar(int index) async{
    Navigator.pop(context);

    if(conectando) return;
    conectando = true;
    try{
      bleRobo = dispsBLE[index];
      //print(bleRobo.id);
      flutterReactiveBle.connectToDevice(id: bleRobo.id).listen((cnx) async{
        if(cnx.connectionState == DeviceConnectionState.connected){
          //Trocando prioridade da conexao
          await flutterReactiveBle.requestConnectionPriority(deviceId: bleRobo.id, priority: ConnectionPriority.highPerformance);
          final characteristic = QualifiedCharacteristic(serviceId: SERVICE_ID, characteristicId: CHAR_E_ID, deviceId: bleRobo.id);
          //Definindo funcao para ser executada a cada atualizacao
          flutterReactiveBle.subscribeToCharacteristic(characteristic).listen((data) {
              readBLE(data);
          }, onError: (dynamic error) {
            print(error);
          });
          conectadoEvent();
          
        }
        else if(cnx.connectionState == DeviceConnectionState.connecting){
            conectandoEvent();
        }
        else if(cnx.connectionState == DeviceConnectionState.disconnected){
          conectado = false;
          conectando = false;
          /*if(escaneando == false){
            escaneando = true;
            listening = flutterReactiveBle.scanForDevices(withServices: [], scanMode: ScanMode.lowLatency).listen((device) {
              if(!entries.contains(device.name)){
                entries.add(device.name);
                dispsBLE.add(device);
              }
              if(conectando == true || (conectado == true)){
                listening.cancel();
                escaneando = false;
                print("Cancelado");
              }
            });
          }*/
          btnConnectController.error();
          Future.delayed(Duration(milliseconds: 2500), (){btnConnectController.stop();});
        }
      });
      
    }
    catch(erro){
      btnConnectController.error();
      showMsg(erro.toString(), context);
      Future.delayed(Duration(milliseconds: 2500), (){btnConnectController.stop();});
    }
    setState((){});
    conectando = false;
  }


  void sendWIFI(String cmd){
    var DESTINATION_ADDRESS=InternetAddress("192.168.4.1");
    List<int> data =utf8.encode(cmd);
    socket?.send(data, DESTINATION_ADDRESS, 8889);
  }
  void readWIFI(data) async{
    if(lendo) return;
    lendo = true;
    if(data == ""){
      lendo = false;
      return;
    }
    serial.text += data.toString().replaceAll(";;;", "");
    for(String _cmd in data.toString().split(";;;")){
      if(_cmd == "");
      else if(_cmd[0] == "T" && (_cmd[1] == ":")){
        receiveConsts(_cmd);
      }
    }
    lendo = false;
    setState(() {});
  }
  
  void sendBLE(String cmd) async{
    if(conectado){
      if(enviando){
        
        if(cmd.contains("J:")){
          if(FilaCMD.length >= 2){
            FilaCMD.remove(FilaCMD.first);
          }
          FilaCMD.add(cmd);
        }
        return;
      } 
      enviando = true;
      final characteristic = QualifiedCharacteristic(serviceId: SERVICE_ID, characteristicId: CHAR_S_ID, deviceId: bleRobo.id); 
      await flutterReactiveBle.writeCharacteristicWithResponse(characteristic, value: utf8.encode(cmd));
      enviando = false;
      if(FilaCMD.isEmpty == false){
          sendCMD(FilaCMD.first);
          FilaCMD.remove(FilaCMD.first);
      }
    }
    else{
      connectDialog();
    }
  }
  void readBLE(data) async{
    if(lendo) return;
    lendo = true;
    final characteristic = QualifiedCharacteristic(serviceId: SERVICE_ID, characteristicId: CHAR_E_ID, deviceId: bleRobo.id);
    final response = await flutterReactiveBle.readCharacteristic(characteristic);
    data = utf8.decode(response);
    if(data == ""){
      lendo = false;
      return;
    }
    serial.text += data.toString().replaceAll(";;;", "");
    for(String _cmd in data.toString().split(";;;")){
      if(_cmd == "");
      else if(_cmd[0] == "T" && (_cmd[1] == ":")){
        receiveConsts(_cmd);
      }
    }
    lendo = false;
    setState(() {});
  }


  //Funcoes auxiliares bluetooth

  void connectDialog() async{
    btnConnectController.start();
    //Desconectar
    if(conectado){
      conectado = false;
      return;
    }

    //Escolher Dispositivo
    Widget lista = await listViewDisps();
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Dispositivos Pareados'),
          content: lista,
        );
    }).then((value) => {if(conectando == false)btnConnectController.stop()});
    
  }
  
  //Envio de comandos para o serial
  void sendCMD(String typeCMD){
    String cmd = "";
    if(typeCMD == "S"){
      cmd = "S: KP" + KP.text + ",KI" + KI.text + ",KD" + KD.text +
      ",VLMIN" + VEL_MIN.text + ",VLMAX" + VEL_MAX.text + ",TMPFR" + TMP_FORA.text
      + ",K" + K.text + ",TOL" + TOL.text
      + ",USARMAPA" + (USE_MAP == true ? "1" : "0") + ",MAPA" + mapa.text;
    }
    else if(typeCMD == "R") 
      {cmd = "R;";}
    else if(typeCMD == "P") 
      {cmd = "P;";}
    else if(typeCMD == "C") 
      {cmd = "C;";}
    else if(typeCMD == "T") 
      {cmd = "T;";}
    else if(typeCMD == "L") 
      {cmd = "L;";}
    else if(typeCMD == "F") 
      {cmd = "F;";}
    else if(typeCMD == "E") 
      {cmd = "E;";}
    else if(typeCMD[0] == 'J' && (conectado)) 
      {cmd = typeCMD;}

    if(cmd != ""){
      if(tipoCnx == "WIFI"){
        sendWIFI(cmd);
      }
      else{
        sendBLE(cmd);
      }
    }
    setState(() {});
  }

  //recebimento das constantes
  void receiveConsts(String consts){
    List<String> nums = consts.split("tes: ")[1].split("|");
    if(KP.text == ""){
      KP.text = nums[0];
      KI.text = nums[1];
      KD.text = nums[2];
      VEL_MIN.text = nums[3];
      VEL_MAX.text = nums[4];
      TMP_FORA.text = nums[5];
      K.text = nums[6];
      TOL.text = nums[7];
    }
    KP_robot = nums[0];
    KI_robot = nums[1];
    KD_robot = nums[2];
    VEL_MIN_robot = nums[3];
    VEL_MAX_robot = nums[4];
    TMP_FORA_robot = nums[5];
    K_robot = nums[6];
    TOL_robot = nums[7];
    if(nums[8] == "1") {
      USE_MAP = true;
    }
    else {USE_MAP = false;}
    mapa.text = nums[9];
    setState((){});
  }

  //Armazenamento de dados - Hive

  void saveConsts() async{

    
    List<String> ops = [KP.text, KI.text, KD.text, VEL_MIN.text, VEL_MAX.text, TMP_FORA.text, K.text, TOL.text, mapa.text];
    
    TextEditingController boxName = TextEditingController();
    Widget cancelButton = TextButton( 
      child: Text("Cancelar"), 
      onPressed: () {Navigator.pop(context, "no");}, 
    ); 
    Widget createNewConfigButton = TextButton( 
      child: Text("Criar nova"), 
      onPressed: () {Navigator.pop(context, boxName.text);}, 
    ); 
    Widget continueButton = TextButton( 
      child: Text("Sim"), 
      onPressed: () {Navigator.pop(context, "yes");}, 
    );
    String pergunta = "Qual o nome da configuração?";

    List<Widget> acoes = [];
    if(await Hive.boxExists(actualBox.replaceAll(RegExp('[^A-Za-z0-9]'), '.'))){
      pergunta = "Deseja substituir a configuração \"$actualBox\"?";
      acoes = [cancelButton, createNewConfigButton, continueButton];
    }
    else{
      acoes =[ cancelButton, createNewConfigButton];
    }

    // configura o AlertDialog 
    AlertDialog alert = AlertDialog( 
      title: Text("Salvar configuração"), 
      content: Column(mainAxisSize: MainAxisSize.min,children: [Text(pergunta), TextField(controller: boxName,)]), 
      actions: acoes 
    ) ;
    // mostra a caixa de diálogo 
    showDialog( 
      context: context, 
      builder: (BuildContext context) { 
        return alert; 
      }, 
    ).then((value) async {
      if(value == "yes"){
        HiveConfig.saveConsts(actualBox, ops);
      }
      else if(value != "no"){
        if(await HiveConfig.saveConsts(value ?? "", ops)){
          actualBox = value;
        }
        
      }
      
    }); 
  }

  void getSavedConsts() async{
    late Box configs;
    configs = await Hive.openBox("configs");
    
    List<dynamic> configsName = configs.keys.toList();

    if(configsName.isEmpty){
      showMsg("Não há configurações salvas", context);
      return;
    }

    String item(dynamic key){
      DateTime date = configs.get(key);

      String hora = (date.hour.toString().length < 2 ? "0" : "") + date.hour.toString();
      String minuto = (date.minute.toString().length < 2 ? "0" : "") + date.minute.toString();
      String dia = (date.day.toString().length < 2 ? "0" : "") + date.day.toString();
      String mes = (date.month.toString().length < 2 ? "0" : "") + date.month.toString();
      return "$key \n   -> $hora:$minuto $dia/$mes/${date.year}";
    }

    Widget conteudo = SizedBox(
      height: 300.0, // Change as per your requirement
      width: 300.0,
      child: ListView.builder(
        scrollDirection: Axis.vertical,
        shrinkWrap: true,
        padding: const EdgeInsets.all(8),
        itemCount: configsName.length,
        itemBuilder: (BuildContext context, int index) {
          return TextButton(
            onPressed: () => {Navigator.pop(context, configsName[index].toString())},
            child: Center(child: Text(item(configsName[index]), style: TextStyle(fontSize: 20, color: Colors.black),)),
          );
      }));
    
    AlertDialog alert = AlertDialog( 
      title: Text("Abrir configuração"), 
      content: conteudo
    ) ;
    // mostra a caixa de diálogo 
    showDialog( 
      context: context, 
      builder: (BuildContext context) { 
        return alert; 
      }, 
    ).then((value) async {
      if((value ?? "") != ""){
        actualBox = value;
        late Box consts;
        consts = await Hive.openBox(actualBox.replaceAll(RegExp('[^A-Za-z0-9]'), '.'));

        KP.text = consts.get("KP") ?? "";
        KI.text = consts.get("KI") ?? "";
        KD.text = consts.get("KD") ?? "";
        VEL_MIN.text = consts.get("VEL_MIN") ?? "";
        VEL_MAX.text = consts.get("VEL_MAX") ?? "";
        TMP_FORA.text = consts.get("TMP_FORA") ?? "";
        K.text = consts.get("K") ?? "";
        TOL.text = consts.get("TOL") ?? "";
        mapa.text = consts.get("MAPA") ?? "";
      }
    }); 

    

  }
  
  void rmConsts() async{

    if(actualBox == ""){
      showMsg("Nenhuma configuração selecionada!", context);
      return;
    }
    Widget cancelButton = TextButton( 
      child: Text("Cancelar"), 
      onPressed: () {Navigator.pop(context, "no");}, 
    ); 
    Widget continueButton = TextButton( 
      child: Text("Sim"), 
      onPressed: () {Navigator.pop(context, "yes");}, 
    );

    List<Widget> acoes = [cancelButton, continueButton];
    AlertDialog alert = AlertDialog( 
      title: Text("Deletar configuração"), 
      content: Text("Tem certeza que deseja remover \"$actualBox\""), 
      actions: acoes 
    ) ;

    
    // mostra a caixa de diálogo 
    showDialog( 
      context: context, 
      builder: (BuildContext context) { 
        return alert; 
      }, 
    ).then((value) {
      if(value == "yes"){
        HiveConfig.removeBox(actualBox);
        actualBox = "";
      }

    });
  }

  

  //Tabs
  
  Tab bluetoothTab(){
    //entries.clear();
    if(conectando == false && (conectado == false)){
      escaneando = true;
      listening = flutterReactiveBle.scanForDevices(withServices: [], scanMode: ScanMode.lowLatency).listen((device) {
          if(!entries.contains(device.name)){
            entries.add(device.name ?? "<Sem nome>");
            dispsBLE.add(device);
          }
          if(conectando == true || (conectado == true)){
            listening.cancel();
            escaneando = false;
          }
        });
      
    }
    List<Widget> elementos = [
        Column(children: [
          //Serial textfield
          Container(
            padding: EdgeInsets.all(7.0),

            child: ConstrainedBox(
              constraints: BoxConstraints(
                minHeight: 50,
                maxHeight: 130,
              ),

              child: SingleChildScrollView(
                scrollDirection: Axis.vertical,
                reverse: true,

                child:TextField(
                  keyboardType: TextInputType.multiline,
                  maxLines: null,
                  controller: serial,
                  decoration: InputDecoration(
                    enabledBorder: OutlineInputBorder(borderSide: BorderSide(color: Color.fromARGB(255, 0, 0, 0))),
                    focusedBorder: OutlineInputBorder(borderSide: BorderSide(color: Color.fromARGB(255, 0, 85, 255)))
                  ),
                )
              )
            ),
          ),
          
          /*Padding(
            padding: EdgeInsets.all(8),
            child: Row( children: [
              fillButton( () => sendCMD("C"), Text("CALIBRAÇÃO", style: TextStyle(fontSize: 20),), 30),
              /*SizedBox(width: 10,),
              fillButton( () => sendCMD("T"), Text("RECEBE CONST", style: TextStyle(fontSize: 20),), 30),*/
              ])
          ),*/
          
          Row(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.start,
            
            children: [
              colunaMetadeTela([
                campoConst("KP", KP, vlAtual: KP_robot),
                campoConst("KI", KI, vlAtual: KI_robot),
                campoConst("KD", KD, vlAtual: KD_robot),
                //campoConst("K", K, vlAtual: K_robot),
              ]),
              colunaMetadeTela([
                campoConst("VEL MAX", VEL_MAX, vlAtual: VEL_MAX_robot),
                campoConst("VEL MIN", VEL_MIN, vlAtual: VEL_MIN_robot),
                campoConst("Marcações", TMP_FORA, vlAtual: TMP_FORA_robot),
                //campoConst("TOL", TOL, vlAtual: TOL_robot),
              ]),
              //ElevatedButton(onPressed: () => {serial.text += "\n\n\n\n\n\n\n\n\n\n\n\nTeste"}, child: Text("Testar Serial"))
            ],
          ),
          //campoConst("MAPA", MAPA, vlAtual: ""),
          Padding(
            padding: EdgeInsets.all(8),
            child: CheckboxListTile(
              value: USE_MAP,
              shape: Border.all(),
              onChanged: (value) => setState((){USE_MAP = (value ?? false);}),
              title: Text("Usar Mapa"),
            )
          ),

          Padding(
            padding: EdgeInsets.all(8),
            child: Row( 
              mainAxisAlignment: MainAxisAlignment.end,
              children: [
                testButton(),
                Expanded(child: Text("")),
                IconButton(onPressed: () => saveConsts(), icon: Icon(Icons.save),iconSize: 40,),
                SizedBox(width: 5),
                IconButton(onPressed: () => getSavedConsts(), icon: Icon(Icons.list),iconSize: 40,),
                SizedBox(width: 5),
                IconButton(onPressed: () => rmConsts(), icon: Icon(Icons.remove_circle_outline),iconSize: 40,)
              ]
            )
          ),
        ],
        ),
        
        
        //ElevatedButton(onPressed: () {mapa.text = "Teste";}, child: Text("teste"))
        
      ];
    return Tab(
      child: 
      Column(children: [
          Expanded(
              child:ListView(
                children: [Padding(
                  padding: EdgeInsets.fromLTRB(0, 10, 0,0),
                    child: Column(
                      children: elementos,
                          
                    ),
                  )
                
              ],
            )
          ),
          //Expanded(child: Text("")),
        Row(
          mainAxisAlignment: MainAxisAlignment.spaceEvenly,
          children: [
            fillButton(() => sendCMD("C"), Text("CLB",style: TextStyle(fontSize: 18, color: corBotAzul, fontWeight: FontWeight.bold)), 0),
            fillButton(() => sendCMD("S"), Text("SET",style: TextStyle(fontSize: 18, color: corBotAzul, fontWeight: FontWeight.bold)), 0),
            fillButton(() => sendCMD("R"), Text("RUN",style: TextStyle(fontSize: 18, color: corBotAzul, fontWeight: FontWeight.bold),), 0),
            fillButton(() => sendCMD("P"), Text("STOP",style: TextStyle(fontSize: 18, color: corBotAzul, fontWeight: FontWeight.bold),), 0),
            
          ],
        ),
        ]
      )
      );
  }
  
  Tab mapTab(){
    return Tab(child: Container(
      padding: EdgeInsets.all(8),
      alignment: Alignment.topCenter,
      child:Column(children:[ 
        Container(
          padding: EdgeInsets.all(8),
          child: Text("MAPA:\n\n(0 -> CURVA | 1 -> RETA)",style: TextStyle(fontSize: 20), textAlign: TextAlign.center,)
        ),
        TextField(
          keyboardType: TextInputType.multiline,
          maxLines: 6,
          controller: mapa,
          style: TextStyle(fontSize: 20, letterSpacing: 5, decoration: TextDecoration.none, decorationThickness: 0),
          decoration: InputDecoration(
            
            enabledBorder: OutlineInputBorder(borderSide: BorderSide(color: Color.fromARGB(255, 0, 0, 0))),
            focusedBorder: OutlineInputBorder(borderSide: BorderSide(color: Color.fromARGB(255, 0, 85, 255)))
          ),
        )
      ])
    ) 
  );
  }
  
  Tab joystickTab(){
    return Tab(
      child:
      Column(mainAxisAlignment: MainAxisAlignment.center,
        children: [
        Stack(
          alignment: Alignment.center,
          children: [
            Container(height: 200, width: 200, decoration: BoxDecoration(shape: BoxShape.circle, color: Colors.black),),
            Joystick(
              base: Container(height: 280, width: 280, decoration: BoxDecoration(shape: BoxShape.circle, color: Colors.transparent),),
              stick: Container(height: 130, width: 130, decoration: BoxDecoration(shape: BoxShape.circle, color: corBotAzul),),
              mode: JoystickMode.all,
              period: Duration(milliseconds: 1),
              listener: (details) {
                setState(() {

                  if(_x != step * details.x || _y != step * details.y){
                    sendCMD("J:${step * details.x},${step * details.y},$step;");
                  }
                  _x = 0 + step * details.x;
                  _y = 0 + step * details.y;
                });
              },
              onStickDragEnd: (){
                sendCMD("J:0,0,$step;");
              },
            ),
          ],
        ),
        //Text("X: ${_x.toString()}"),
        //Text("Y: ${_y.toString()}"),
      ],
    ));
  }
  
  @override
  Widget build(BuildContext context) {
    return WillPopScope(
      onWillPop: backAndroid,
      child: SafeArea(
        child: Scaffold(
          appBar: AppBar(
            backgroundColor: Colors.black,
            title: TabBar(
              controller: tabController,
              labelColor: corBotAzul,
              //dividerColor: Colors.black,
              indicatorColor: corBotAzul,//Colors.white,
              tabs:[
                Padding(padding: EdgeInsets.only(bottom: 10), child: 
                  LoadingButton(
                    onPressed: () => {tabController.animateTo(0)},
                    controller: btnConnectController,
                    valueColor: corBotAzul,
                    loaderStrokeWidth: 3,
                    failedIcon: Icons.close,
                    height: 30,
                    width: 30,
                    primaryColor: Color.fromARGB(255, 0, 0, 0),
                    animateOnTap: false,
                    child: Icon(tipoCnx == "WIFI"? Icons.wifi : Icons.bluetooth, color: corBotAzul,),
                    
                  )
                ),
                
                Padding(padding: EdgeInsets.only(bottom: 10), child: Icon(Icons.map)),
                
                Padding(padding: EdgeInsets.only(bottom: 10), child: Icon(Icons.gamepad)),
                
              ],
            ),
          ),
          body: 
            TabBarView(
              controller: tabController,
              physics: NeverScrollableScrollPhysics(),
              children: [
              
                bluetoothTab(),
                mapTab(),
                joystickTab(),

              ]
            ,)
          )
        )
      );
  }
}

