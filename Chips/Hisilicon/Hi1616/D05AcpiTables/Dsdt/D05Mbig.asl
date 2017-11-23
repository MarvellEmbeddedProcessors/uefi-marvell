/** @file
  Differentiated System Description Table Fields (DSDT)

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>
  Copyright (c) 2015-2016, Hisilicon Limited. All rights reserved.
    This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

Scope(_SB)
{
  // Mbi-gen peri b intc
  Device(MBI0) {
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0x60080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive, 0, ,) { 807 }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 1}
      }
    })
  }

  Device(MBI1) {
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xc0080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588,
        589, 590, 591, 592, 593, 594, 595, 596, 597, 598, 599, 600,
      }
      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        960, 961, 962, 963, 964, 965, 966, 967, 968, 969, 970, 971, 972, 973, 974, 975,
        976, 977, 978, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 991,
        992, 993, 994, 995, 996, 997, 998, 999, 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007,
        1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023,
        1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038, 1039,
        1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055,
        1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071,
        1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087,
        1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103,
        1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119,
        1120, 1121, 1122, 1123, 1124, 1125, 1126, 1127, 1128, 1129, 1130, 1131, 1132, 1133, 1134, 1135,
        1136, 1137, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1145, 1146, 1147, 1148, 1149, 1150, 1151,
      }
      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167,
        1168, 1169, 1170, 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183,
        1184, 1185, 1186, 1187, 1188, 1189, 1190, 1191, 1192, 1193, 1194, 1195, 1196, 1197, 1198, 1199,
        1200, 1201, 1202, 1203, 1204, 1205, 1206, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1214, 1215,
        1216, 1217, 1218, 1219, 1220, 1221, 1222, 1223, 1224, 1225, 1226, 1227, 1228, 1229, 1230, 1231,
        1232, 1233, 1234, 1235, 1236, 1237, 1238, 1239, 1240, 1241, 1242, 1243, 1244, 1245, 1246, 1247,
        1248, 1249, 1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257, 1258, 1259, 1260, 1261, 1262, 1263,
        1264, 1265, 1266, 1267, 1268, 1269, 1270, 1271, 1272, 1273, 1274, 1275, 1276, 1277, 1278, 1279,
        1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295,
        1296, 1297, 1298, 1299, 1300, 1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311,
        1312, 1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327,
        1328, 1329, 1330, 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343,
      }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 409}
      }
    })
  }

  // Mbi-gen sas0
  Device(MBI2) {
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xc0080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive, 0, ,)
      {
        64,65,66,67,68,
        69,70,71,72,73,
        75,76,77,78,79,
        80,81,82,83,84,
        85,86,87,88,89,
        90,91,92,93,94,
        95,96,97,98,99,
        100,101,102,103,104,
        105,106,107,108,109,
        110,111,112,113,114,
        115,116,117,118,119,
        120,121,122,123,124,
        125,126,127,128,129,
        130,131,132,133,134,
        135,136,137,138,139,
        140,141,142,143,144,
        145,146,147,148,149,
        150,151,152,153,154,
        155,156,157,158,159,
        160,
      }

      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0,,)
      {
        601,602,603,604,
        605,606,607,608,609,
        610,611,612,613,614,
        615,616,617,618,619,
        620,621,622,623,624,
        625,626,627,628,629,
        630,631,632,
      }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 128}
      }
    })
  }

  Device(MBI3) {          // Mbi-gen sas1 intc
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xa0080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive, 0, ,)
      {
        64,65,66,67,68,
        69,70,71,72,73,
        75,76,77,78,79,
        80,81,82,83,84,
        85,86,87,88,89,
        90,91,92,93,94,
        95,96,97,98,99,
        100,101,102,103,104,
        105,106,107,108,109,
        110,111,112,113,114,
        115,116,117,118,119,
        120,121,122,123,124,
        125,126,127,128,129,
        130,131,132,133,134,
        135,136,137,138,139,
        140,141,142,143,144,
        145,146,147,148,149,
        150,151,152,153,154,
        155,156,157,158,159,
        160,
      }

      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        576,577,578,579,580,
        581,582,583,584,585,
        586,587,588,589,590,
        591,592,593,594,595,
        596,597,598,599,600,
        601,602,603,604,605,
        606,607,
      }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 128}
      }
    })
  }
  Device(MBI4) {          // Mbi-gen sas2 intc
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xa0080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive, 0, ,)
      {
        192,193,194,195,196,
        197,198,199,200,201,
        202,203,204,205,206,
        207,208,209,210,211,
        212,213,214,215,216,
        217,218,219,220,221,
        222,223,224,225,226,
        227,228,229,230,231,
        232,233,234,235,236,
        237,238,239,240,241,
        242,243,244,245,246,
        247,248,249,250,251,
        252,253,254,255,256,
        257,258,259,260,261,
        262,263,264,265,266,
        267,268,269,270,271,
        272,273,274,275,276,
        277,278,279,280,281,
        282,283,284,285,286,
        287,
      }

      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        608,609,610,611,
        612,613,614,615,616,
        617,618,619,620,621,
        622,623,624,625,626,
        627,628,629,630,631,
        632,633,634,635,636,
        637,638,639,
      }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 128}
      }
    })
  }

  Device(MBI5) {
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xa0080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive, 0,,) {640,641,}
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 2}
      }
    })
  }

  Device(MBI6) {
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xd0080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive, 0,,) { 705 }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 1}
      }
    })
  }

  Device(MBI7) {
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xd0080000, 0x10000)
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Level, ActiveHigh, Exclusive, 0,,) { 707 }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 1}
      }
    })
  }

  Device(MBI8) {
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      QwordMemory (
        ResourceProducer,
        PosDecode,
        MinFixed,
        MaxFixed,
        NonCacheable,
        ReadWrite,
        0x0, // Granularity
        0x400c0080000, // Min Base Address
        0x400c008ffff, // Max Base Address
        0x0, // Translate
        0x10000 // Length
      )
    })

    Name(_PRS, ResourceTemplate() {
      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588,
        589, 590, 591, 592, 593, 594, 595, 596, 597, 598, 599, 600,
      }
      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        960, 961, 962, 963, 964, 965, 966, 967, 968, 969, 970, 971, 972, 973, 974, 975,
        976, 977, 978, 979, 980, 981, 982, 983, 984, 985, 986, 987, 988, 989, 990, 991,
        992, 993, 994, 995, 996, 997, 998, 999, 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007,
        1008, 1009, 1010, 1011, 1012, 1013, 1014, 1015, 1016, 1017, 1018, 1019, 1020, 1021, 1022, 1023,
        1024, 1025, 1026, 1027, 1028, 1029, 1030, 1031, 1032, 1033, 1034, 1035, 1036, 1037, 1038, 1039,
        1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055,
        1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071,
        1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087,
        1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103,
        1104, 1105, 1106, 1107, 1108, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119,
        1120, 1121, 1122, 1123, 1124, 1125, 1126, 1127, 1128, 1129, 1130, 1131, 1132, 1133, 1134, 1135,
        1136, 1137, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1145, 1146, 1147, 1148, 1149, 1150, 1151,
      }
      Interrupt (ResourceProducer, Edge, ActiveHigh, Exclusive, 0, ,)
      {
        1152, 1153, 1154, 1155, 1156, 1157, 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167,
        1168, 1169, 1170, 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179, 1180, 1181, 1182, 1183,
        1184, 1185, 1186, 1187, 1188, 1189, 1190, 1191, 1192, 1193, 1194, 1195, 1196, 1197, 1198, 1199,
        1200, 1201, 1202, 1203, 1204, 1205, 1206, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1214, 1215,
        1216, 1217, 1218, 1219, 1220, 1221, 1222, 1223, 1224, 1225, 1226, 1227, 1228, 1229, 1230, 1231,
        1232, 1233, 1234, 1235, 1236, 1237, 1238, 1239, 1240, 1241, 1242, 1243, 1244, 1245, 1246, 1247,
        1248, 1249, 1250, 1251, 1252, 1253, 1254, 1255, 1256, 1257, 1258, 1259, 1260, 1261, 1262, 1263,
        1264, 1265, 1266, 1267, 1268, 1269, 1270, 1271, 1272, 1273, 1274, 1275, 1276, 1277, 1278, 1279,
        1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290, 1291, 1292, 1293, 1294, 1295,
        1296, 1297, 1298, 1299, 1300, 1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311,
        1312, 1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327,
        1328, 1329, 1330, 1331, 1332, 1333, 1334, 1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343,
      }
    })

    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 409}
      }
    })
  }
/*
  Device(MBI4) {          // Mbi-gen dsa1 dbg0 intc
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xc0080000, 0x10000)
    })
    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 9}
      }
    })
  }

  Device(MBI5) {          // Mbi-gen dsa2 dbg1 intc
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xc0080000, 0x10000)
    })
    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 9}
      }
    })
  }

  Device(MBI6) {          // Mbi-gen dsa sas0 intc
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xc0080000, 0x10000)
    })
    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package ()
      {
        Package () {"num-pins", 128}
      }
    })
  }
*/
  Device(MBI9) {          // Mbi-gen roce intc
    Name(_HID, "HISI0152")
    Name(_CID, "MBIGen")
    Name(_CRS, ResourceTemplate() {
      Memory32Fixed(ReadWrite, 0xc0080000, 0x10000)
    })
    Name (_PRS, ResourceTemplate (){
      Interrupt (ResourceConsumer, Edge, ActiveHigh, Exclusive,,,)
      {
        722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733,
        734, 735, 736, 737, 738, 739, 740, 741, 742, 743, 744, 745,
        746, 747, 748, 749, 750, 751, 752, 753, 785, 754,
      }
    })
  }
}
