import 'dart:io';

import 'package:hive/hive.dart';
import 'package:hive_flutter/adapters.dart';
import 'package:path_provider/path_provider.dart';

class HiveConfig{
  static start() async{
    Directory dir = await getApplicationDocumentsDirectory();
    await Hive.initFlutter(dir.path);
  }

  static String validBoxName(String boxName){
    return boxName.replaceAll(RegExp('[^A-Za-z0-9]'), '.');
  }

  static Future<bool> saveConsts(String boxName, List<String> options) async{
    if(boxName.isEmpty) return false;

    late Box configs;
    configs = await Hive.openBox("configs");
    configs.put(boxName, DateTime.now());
    configs.close();
    
    late Box consts;
    consts = await Hive.openBox(boxName.replaceAll(RegExp('[^A-Za-z0-9]'), '.'));

    consts.put("KP", options[0]);
    consts.put("KI", options[1]);
    consts.put("KD", options[2]);
    consts.put("VEL_MIN", options[3]);
    consts.put("VEL_MAX", options[4]);
    consts.put("TMP_FORA", options[5]);
    consts.put("MAPA", options[6]);
    return true;
  }

  static Future<List<String>> getSavedConsts(String boxName) async{
    late Box consts;
    consts = await Hive.openBox(boxName.replaceAll(RegExp('[^A-Za-z0-9]'), '.'));
    return[ consts.get("KP"), consts.get("KI"), consts.get("KD"),consts.get("VEL_MIN"),
            consts.get("VEL_MAX"),consts.get("TMP_FORA"), consts.get("MAPA")];

  }

  static Future<bool> removeBox(String boxName) async{
    if(boxName.isEmpty) return false;
    if(await Hive.boxExists(boxName.replaceAll(RegExp('[^A-Za-z0-9]'), '.'))){
      late Box box;
      box = await Hive.openBox(boxName.replaceAll(RegExp('[^A-Za-z0-9]'), '.'));
      await box.deleteFromDisk();

      box = await Hive.openBox("configs");
      box.delete(boxName);
      return true;
    }
    return false;
  }
}