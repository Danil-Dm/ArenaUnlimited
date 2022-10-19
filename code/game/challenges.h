/*
===========================================================================
Copyright (C) 2008-2009 Poul Sander

This file is part of Open Arena source code.

Open Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Open Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Open Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/**
 * This is a list of challenges that a player can only complete while playing
 * against human opponents, either LAN or Internet.
 *
 * Hopefully this will make it a little more fun to play online even if you
 * are not good enough to win you can still track your progress.
 */

#define CHALLENGES_MAX_COUNT	MAX_INT-1
#define CHALLENGES_MAX          2048

#define GENERAL_TEST		0
#define GENERAL_TOTALKILLS      1
#define GENERAL_TOTALDEATHS     2
#define GENERAL_MONEY      3

//Gametypes
#define GAMETYPES_FFA_WINS      101
#define GAMETYPES_TOURNEY_WINS  102
#define GAMETYPES_TDM_WINS      103
#define GAMETYPES_CTF_WINS      104
#define GAMETYPES_1FCTF_WINS    105
#define GAMETYPES_OVERLOAD_WINS 106
#define GAMETYPES_HARVESTER_WINS 107
#define GAMETYPES_ELIMINATION_WINS 108
#define GAMETYPES_CTF_ELIMINATION_WINS 109
#define GAMETYPES_LMS_WINS      110
#define GAMETYPES_DD_WINS       111
#define GAMETYPES_DOM_WINS      112

//Weapons
#define WEAPON_GAUNTLET_KILLS   201
#define WEAPON_MACHINEGUN_KILLS 202
#define WEAPON_SHOTGUN_KILLS    203
#define WEAPON_GRANADE_KILLS    204
#define WEAPON_ROCKET_KILLS     205
#define WEAPON_LIGHTNING_KILLS  206
#define WEAPON_PLASMA_KILLS     207
#define WEAPON_RAIL_KILLS       208
#define WEAPON_BFG_KILLS        209
#define WEAPON_GRAPPLE_KILLS    210
#define WEAPON_CHAINGUN_KILLS   211
#define WEAPON_NAILGUN_KILLS    212
#define WEAPON_MINE_KILLS       213
#define WEAPON_PUSH_KILLS       214
#define WEAPON_INSTANT_RAIL_KILLS 215
#define WEAPON_TELEFRAG_KILLS   216
#define WEAPON_CRUSH_KILLS      217
#define WEAPON_FLAMETHROWER_KILLS    218
#define WEAPON_DARKFLARE_KILLS    219

//Awards
//Gauntlet is not here as it is the same as WEAPON_GAUNTLET_KILLS
#define AWARD_IMPRESSIVE        301
#define AWARD_EXCELLENT          302
#define AWARD_CAPTURE           303
#define AWARD_ASSIST            304
#define AWARD_DEFENCE           305

//Powerups
#define POWERUP_QUAD_KILL       401
#define POWERUP_SPEED_KILL      402
#define POWERUP_FLIGHT_KILL     403
#define POWERUP_INVIS_KILL      404
#define POWERUP_MULTI_KILL      405
#define POWERUP_COUNTER_QUAD    406
#define POWERUP_COUNTER_SPEED   407
#define POWERUP_COUNTER_FLIGHT  408
#define POWERUP_COUNTER_INVIS   409
#define POWERUP_COUNTER_ENVIR   410
#define POWERUP_COUNTER_REGEN   411
#define POWERUP_COUNTER_MULTI   412

//FFA awards
#define FFA_TOP3                501
//From behind, enemy gets fraglimit-1, player has at most fraglimit-2 but wins anyway
#define FFA_FROMBEHIND          502
//BetterThan: loose a match but have a positive kill ratio against the winner
#define FFA_BETTERTHAN          503
//Get at least half of your kills for players in the best half of the scoreboard
#define FFA_JUDGE               504
//The oppesite
#define FFA_CHEAPKILLER         505

#define	SKIN1	1001
#define	SKIN2	1002
#define	SKIN3	1003
#define	SKIN4	1004
#define	SKIN5	1005
#define	SKIN6	1006
#define	SKIN7	1007
#define	SKIN8	1008
#define	SKIN9	1009
#define	SKIN10	1010
#define	SKIN11	1011
#define	SKIN12	1012
#define	SKIN13	1013
#define	SKIN14	1014
#define	SKIN15	1015
#define	SKIN16	1016
#define	SKIN17	1017
#define	SKIN18	1018
#define	SKIN19	1019
#define	SKIN20	1020
#define	SKIN21	1021
#define	SKIN22	1022
#define	SKIN23	1023
#define	SKIN24	1024
#define	SKIN25	1025
#define	SKIN26	1026
#define	SKIN27	1027
#define	SKIN28	1028
#define	SKIN29	1029
#define	SKIN30	1030
#define	SKIN31	1031
#define	SKIN32	1032
#define	SKIN33	1033
#define	SKIN34	1034
#define	SKIN35	1035
#define	SKIN36	1036
#define	SKIN37	1037
#define	SKIN38	1038
#define	SKIN39	1039
#define	SKIN40	1040
#define	SKIN41	1041
#define	SKIN42	1042
#define	SKIN43	1043
#define	SKIN44	1044
#define	SKIN45	1045
#define	SKIN46	1046
#define	SKIN47	1047
#define	SKIN48	1048
#define	SKIN49	1049
#define	SKIN50	1050
#define	SKIN51	1051
#define	SKIN52	1052
#define	SKIN53	1053
#define	SKIN54	1054
#define	SKIN55	1055
#define	SKIN56	1056
#define	SKIN57	1057
#define	SKIN58	1058
#define	SKIN59	1059
#define	SKIN60	1060
#define	SKIN61	1061
#define	SKIN62	1062
#define	SKIN63	1063
#define	SKIN64	1064
#define	SKIN65	1065
#define	SKIN66	1066
#define	SKIN67	1067
#define	SKIN68	1068
#define	SKIN69	1069
#define	SKIN70	1070
#define	SKIN71	1071
#define	SKIN72	1072
#define	SKIN73	1073
#define	SKIN74	1074
#define	SKIN75	1075
#define	SKIN76	1076
#define	SKIN77	1077
#define	SKIN78	1078
#define	SKIN79	1079
#define	SKIN80	1080
#define	SKIN81	1081
#define	SKIN82	1082
#define	SKIN83	1083
#define	SKIN84	1084
#define	SKIN85	1085
#define	SKIN86	1086
#define	SKIN87	1087
#define	SKIN88	1088
#define	SKIN89	1089
#define	SKIN90	1090
#define	SKIN91	1091
#define	SKIN92	1092
#define	SKIN93	1093
#define	SKIN94	1094
#define	SKIN95	1095
#define	SKIN96	1096
#define	SKIN97	1097
#define	SKIN98	1098
#define	SKIN99	1099
#define	SKIN100	1100
#define	SKIN101	1101
#define	SKIN102	1102
#define	SKIN103	1103
#define	SKIN104	1104
#define	SKIN105	1105
#define	SKIN106	1106
#define	SKIN107	1107
#define	SKIN108	1108
#define	SKIN109	1109
#define	SKIN110	1110
#define	SKIN111	1111
#define	SKIN112	1112
#define	SKIN113	1113
#define	SKIN114	1114
#define	SKIN115	1115
#define	SKIN116	1116
#define	SKIN117	1117
#define	SKIN118	1118
#define	SKIN119	1119
#define	SKIN120	1120
#define	SKIN121	1121
#define	SKIN122	1122
#define	SKIN123	1123
#define	SKIN124	1124
#define	SKIN125	1125
#define	SKIN126	1126
#define	SKIN127	1127
#define	SKIN128	1128
#define	SKIN129	1129
#define	SKIN130	1130
#define	SKIN131	1131
#define	SKIN132	1132
#define	SKIN133	1133
#define	SKIN134	1134
#define	SKIN135	1135
#define	SKIN136	1136
#define	SKIN137	1137
#define	SKIN138	1138
#define	SKIN139	1139
#define	SKIN140	1140
#define	SKIN141	1141
#define	SKIN142	1142
#define	SKIN143	1143
#define	SKIN144	1144
#define	SKIN145	1145
#define	SKIN146	1146
#define	SKIN147	1147
#define	SKIN148	1148
#define	SKIN149	1149
#define	SKIN150	1150
#define	SKIN151	1151
#define	SKIN152	1152
#define	SKIN153	1153
#define	SKIN154	1154
#define	SKIN155	1155
#define	SKIN156	1156
#define	SKIN157	1157
#define	SKIN158	1158
#define	SKIN159	1159
#define	SKIN160	1160
#define	SKIN161	1161
#define	SKIN162	1162
#define	SKIN163	1163
#define	SKIN164	1164
#define	SKIN165	1165
#define	SKIN166	1166
#define	SKIN167	1167
#define	SKIN168	1168
#define	SKIN169	1169
#define	SKIN170	1170
#define	SKIN171	1171
#define	SKIN172	1172
#define	SKIN173	1173
#define	SKIN174	1174
#define	SKIN175	1175
#define	SKIN176	1176
#define	SKIN177	1177
#define	SKIN178	1178
#define	SKIN179	1179
#define	SKIN180	1180
#define	SKIN181	1181
#define	SKIN182	1182
#define	SKIN183	1183
#define	SKIN184	1184
#define	SKIN185	1185
#define	SKIN186	1186
#define	SKIN187	1187
#define	SKIN188	1188
#define	SKIN189	1189
#define	SKIN190	1190
#define	SKIN191	1191
#define	SKIN192	1192
#define	SKIN193	1193
#define	SKIN194	1194
#define	SKIN195	1195
#define	SKIN196	1196
#define	SKIN197	1197
#define	SKIN198	1198
#define	SKIN199	1199
#define	SKIN200	1200
#define	SKIN201	1201
#define	SKIN202	1202
#define	SKIN203	1203
#define	SKIN204	1204
#define	SKIN205	1205
#define	SKIN206	1206
#define	SKIN207	1207
#define	SKIN208	1208
#define	SKIN209	1209
#define	SKIN210	1210
#define	SKIN211	1211
#define	SKIN212	1212
#define	SKIN213	1213
#define	SKIN214	1214
#define	SKIN215	1215
#define	SKIN216	1216
#define	SKIN217	1217
#define	SKIN218	1218
#define	SKIN219	1219
#define	SKIN220	1220
#define	SKIN221	1221
#define	SKIN222	1222
#define	SKIN223	1223
#define	SKIN224	1224
#define	SKIN225	1225
#define	SKIN226	1226
#define	SKIN227	1227
#define	SKIN228	1228
#define	SKIN229	1229
#define	SKIN230	1230
#define	SKIN231	1231
#define	SKIN232	1232
#define	SKIN233	1233
#define	SKIN234	1234
#define	SKIN235	1235
#define	SKIN236	1236
#define	SKIN237	1237
#define	SKIN238	1238
#define	SKIN239	1239
#define	SKIN240	1240
#define	SKIN241	1241
#define	SKIN242	1242
#define	SKIN243	1243
#define	SKIN244	1244
#define	SKIN245	1245
#define	SKIN246	1246
#define	SKIN247	1247
#define	SKIN248	1248
#define	SKIN249	1249
#define	SKIN250	1250
#define	SKIN251	1251
#define	SKIN252	1252
#define	SKIN253	1253
#define	SKIN254	1254
#define	SKIN255	1255
#define	SKIN256	1256
#define	SKIN257	1257
#define	SKIN258	1258
#define	SKIN259	1259
#define	SKIN260	1260
#define	SKIN261	1261
#define	SKIN262	1262
#define	SKIN263	1263
#define	SKIN264	1264
#define	SKIN265	1265
#define	SKIN266	1266
#define	SKIN267	1267
#define	SKIN268	1268
#define	SKIN269	1269
#define	SKIN270	1270
#define	SKIN271	1271
#define	SKIN272	1272
#define	SKIN273	1273
#define	SKIN274	1274
#define	SKIN275	1275
#define	SKIN276	1276
#define	SKIN277	1277
#define	SKIN278	1278
#define	SKIN279	1279
#define	SKIN280	1280
#define	SKIN281	1281
#define	SKIN282	1282
#define	SKIN283	1283
#define	SKIN284	1284
#define	SKIN285	1285
#define	SKIN286	1286
#define	SKIN287	1287
#define	SKIN288	1288
#define	SKIN289	1289
#define	SKIN290	1290
#define	SKIN291	1291
#define	SKIN292	1292
#define	SKIN293	1293
#define	SKIN294	1294
#define	SKIN295	1295
#define	SKIN296	1296
#define	SKIN297	1297
#define	SKIN298	1298
#define	SKIN299	1299
#define	SKIN300	1300
#define	SKIN301	1301
#define	SKIN302	1302
#define	SKIN303	1303
#define	SKIN304	1304
#define	SKIN305	1305
#define	SKIN306	1306
#define	SKIN307	1307
#define	SKIN308	1308
#define	SKIN309	1309
#define	SKIN310	1310
#define	SKIN311	1311
#define	SKIN312	1312
#define	SKIN313	1313
#define	SKIN314	1314
#define	SKIN315	1315
#define	SKIN316	1316
#define	SKIN317	1317
#define	SKIN318	1318
#define	SKIN319	1319
#define	SKIN320	1320
#define	SKIN321	1321
#define	SKIN322	1322
#define	SKIN323	1323
#define	SKIN324	1324
#define	SKIN325	1325
#define	SKIN326	1326
#define	SKIN327	1327
#define	SKIN328	1328
#define	SKIN329	1329
#define	SKIN330	1330
#define	SKIN331	1331
#define	SKIN332	1332
#define	SKIN333	1333
#define	SKIN334	1334
#define	SKIN335	1335
#define	SKIN336	1336
#define	SKIN337	1337
#define	SKIN338	1338
#define	SKIN339	1339
#define	SKIN340	1340
#define	SKIN341	1341
#define	SKIN342	1342
#define	SKIN343	1343
#define	SKIN344	1344
#define	SKIN345	1345
#define	SKIN346	1346
#define	SKIN347	1347
#define	SKIN348	1348
#define	SKIN349	1349
#define	SKIN350	1350
#define	SKIN351	1351
#define	SKIN352	1352
#define	SKIN353	1353
#define	SKIN354	1354
#define	SKIN355	1355
#define	SKIN356	1356
#define	SKIN357	1357
#define	SKIN358	1358
#define	SKIN359	1359
#define	SKIN360	1360
#define	SKIN361	1361
#define	SKIN362	1362
#define	SKIN363	1363
#define	SKIN364	1364
#define	SKIN365	1365
#define	SKIN366	1366
#define	SKIN367	1367
#define	SKIN368	1368
#define	SKIN369	1369
#define	SKIN370	1370
#define	SKIN371	1371
#define	SKIN372	1372
#define	SKIN373	1373
#define	SKIN374	1374
#define	SKIN375	1375
#define	SKIN376	1376
#define	SKIN377	1377
#define	SKIN378	1378
#define	SKIN379	1379
#define	SKIN380	1380
#define	SKIN381	1381
#define	SKIN382	1382
#define	SKIN383	1383
#define	SKIN384	1384
#define	SKIN385	1385
#define	SKIN386	1386
#define	SKIN387	1387
#define	SKIN388	1388
#define	SKIN389	1389
#define	SKIN390	1390
#define	SKIN391	1391
#define	SKIN392	1392
#define	SKIN393	1393
#define	SKIN394	1394
#define	SKIN395	1395
#define	SKIN396	1396
#define	SKIN397	1397
#define	SKIN398	1398
#define	SKIN399	1399
#define	SKIN400	1400
#define	SKIN401	1401
#define	SKIN402	1402
#define	SKIN403	1403
#define	SKIN404	1404
#define	SKIN405	1405
#define	SKIN406	1406
#define	SKIN407	1407
#define	SKIN408	1408
#define	SKIN409	1409
#define	SKIN410	1410
#define	SKIN411	1411
#define	SKIN412	1412
#define	SKIN413	1413
#define	SKIN414	1414
#define	SKIN415	1415
#define	SKIN416	1416
#define	SKIN417	1417
#define	SKIN418	1418
#define	SKIN419	1419
#define	SKIN420	1420
#define	SKIN421	1421
#define	SKIN422	1422
#define	SKIN423	1423
#define	SKIN424	1424
#define	SKIN425	1425
#define	SKIN426	1426
#define	SKIN427	1427
#define	SKIN428	1428
#define	SKIN429	1429
#define	SKIN430	1430
#define	SKIN431	1431
#define	SKIN432	1432
#define	SKIN433	1433
#define	SKIN434	1434
#define	SKIN435	1435
#define	SKIN436	1436
#define	SKIN437	1437
#define	SKIN438	1438
#define	SKIN439	1439
#define	SKIN440	1440
#define	SKIN441	1441
#define	SKIN442	1442
#define	SKIN443	1443
#define	SKIN444	1444
#define	SKIN445	1445
#define	SKIN446	1446
#define	SKIN447	1447
#define	SKIN448	1448
#define	SKIN449	1449
#define	SKIN450	1450
#define	SKIN451	1451
#define	SKIN452	1452
#define	SKIN453	1453
#define	SKIN454	1454
#define	SKIN455	1455
#define	SKIN456	1456
#define	SKIN457	1457
#define	SKIN458	1458
#define	SKIN459	1459
#define	SKIN460	1460
#define	SKIN461	1461
#define	SKIN462	1462
#define	SKIN463	1463
#define	SKIN464	1464
#define	SKIN465	1465
#define	SKIN466	1466
#define	SKIN467	1467
#define	SKIN468	1468
#define	SKIN469	1469
#define	SKIN470	1470
#define	SKIN471	1471
#define	SKIN472	1472
#define	SKIN473	1473
#define	SKIN474	1474
#define	SKIN475	1475
#define	SKIN476	1476
#define	SKIN477	1477
#define	SKIN478	1478
#define	SKIN479	1479
#define	SKIN480	1480
#define	SKIN481	1481
#define	SKIN482	1482
#define	SKIN483	1483
#define	SKIN484	1484
#define	SKIN485	1485
#define	SKIN486	1486
#define	SKIN487	1487
#define	SKIN488	1488
#define	SKIN489	1489
#define	SKIN490	1490
#define	SKIN491	1491
#define	SKIN492	1492
#define	SKIN493	1493
#define	SKIN494	1494
#define	SKIN495	1495
#define	SKIN496	1496
#define	SKIN497	1497
#define	SKIN498	1498
#define	SKIN499	1499
#define	SKIN500	1500
#define	SKIN501	1501
#define	SKIN502	1502
#define	SKIN503	1503
#define	SKIN504	1504
#define	SKIN505	1505
#define	SKIN506	1506
#define	SKIN507	1507
#define	SKIN508	1508
#define	SKIN509	1509
#define	SKIN510	1510
#define	SKIN511	1511
#define	SKIN512	1512
#define	SKIN513	1513
#define	SKIN514	1514
#define	SKIN515	1515
#define	SKIN516	1516
#define	SKIN517	1517
#define	SKIN518	1518
#define	SKIN519	1519
#define	SKIN520	1520
#define	SKIN521	1521
#define	SKIN522	1522
#define	SKIN523	1523
#define	SKIN524	1524
#define	SKIN525	1525
#define	SKIN526	1526
#define	SKIN527	1527
#define	SKIN528	1528
#define	SKIN529	1529
#define	SKIN530	1530
#define	SKIN531	1531
#define	SKIN532	1532
#define	SKIN533	1533
#define	SKIN534	1534
#define	SKIN535	1535
#define	SKIN536	1536
#define	SKIN537	1537
#define	SKIN538	1538
#define	SKIN539	1539
#define	SKIN540	1540
#define	SKIN541	1541
#define	SKIN542	1542
#define	SKIN543	1543
#define	SKIN544	1544
#define	SKIN545	1545
#define	SKIN546	1546
#define	SKIN547	1547
#define	SKIN548	1548
#define	SKIN549	1549
#define	SKIN550	1550
#define	SKIN551	1551
#define	SKIN552	1552
#define	SKIN553	1553
#define	SKIN554	1554
#define	SKIN555	1555
#define	SKIN556	1556
#define	SKIN557	1557
#define	SKIN558	1558
#define	SKIN559	1559
#define	SKIN560	1560
#define	SKIN561	1561
#define	SKIN562	1562
#define	SKIN563	1563
#define	SKIN564	1564
#define	SKIN565	1565
#define	SKIN566	1566
#define	SKIN567	1567
#define	SKIN568	1568
#define	SKIN569	1569
#define	SKIN570	1570
#define	SKIN571	1571
#define	SKIN572	1572
#define	SKIN573	1573
#define	SKIN574	1574
#define	SKIN575	1575
#define	SKIN576	1576
#define	SKIN577	1577
#define	SKIN578	1578
#define	SKIN579	1579
#define	SKIN580	1580
#define	SKIN581	1581
#define	SKIN582	1582
#define	SKIN583	1583
#define	SKIN584	1584
#define	SKIN585	1585
#define	SKIN586	1586
#define	SKIN587	1587
#define	SKIN588	1588
#define	SKIN589	1589
#define	SKIN590	1590
#define	SKIN591	1591
#define	SKIN592	1592
#define	SKIN593	1593
#define	SKIN594	1594
#define	SKIN595	1595
#define	SKIN596	1596
#define	SKIN597	1597
#define	SKIN598	1598
#define	SKIN599	1599
#define	SKIN600	1600
#define	SKIN601	1601
#define	SKIN602	1602
#define	SKIN603	1603
#define	SKIN604	1604
#define	SKIN605	1605
#define	SKIN606	1606
#define	SKIN607	1607
#define	SKIN608	1608
#define	SKIN609	1609
#define	SKIN610	1610
#define	SKIN611	1611
#define	SKIN612	1612
#define	SKIN613	1613
#define	SKIN614	1614
#define	SKIN615	1615
#define	SKIN616	1616
#define	SKIN617	1617
#define	SKIN618	1618
#define	SKIN619	1619
#define	SKIN620	1620
#define	SKIN621	1621
#define	SKIN622	1622
#define	SKIN623	1623
#define	SKIN624	1624
#define	SKIN625	1625
#define	SKIN626	1626
#define	SKIN627	1627
#define	SKIN628	1628
#define	SKIN629	1629
#define	SKIN630	1630
#define	SKIN631	1631
#define	SKIN632	1632
#define	SKIN633	1633
#define	SKIN634	1634
#define	SKIN635	1635
#define	SKIN636	1636
#define	SKIN637	1637
#define	SKIN638	1638
#define	SKIN639	1639
#define	SKIN640	1640
#define	SKIN641	1641
#define	SKIN642	1642
#define	SKIN643	1643
#define	SKIN644	1644
#define	SKIN645	1645
#define	SKIN646	1646
#define	SKIN647	1647
#define	SKIN648	1648
#define	SKIN649	1649
#define	SKIN650	1650
#define	SKIN651	1651
#define	SKIN652	1652
#define	SKIN653	1653
#define	SKIN654	1654
#define	SKIN655	1655
#define	SKIN656	1656
#define	SKIN657	1657
#define	SKIN658	1658
#define	SKIN659	1659
#define	SKIN660	1660
#define	SKIN661	1661
#define	SKIN662	1662
#define	SKIN663	1663
#define	SKIN664	1664
#define	SKIN665	1665
#define	SKIN666	1666
#define	SKIN667	1667
#define	SKIN668	1668
#define	SKIN669	1669
#define	SKIN670	1670
#define	SKIN671	1671
#define	SKIN672	1672
#define	SKIN673	1673
#define	SKIN674	1674
#define	SKIN675	1675
#define	SKIN676	1676
#define	SKIN677	1677
#define	SKIN678	1678
#define	SKIN679	1679
#define	SKIN680	1680
#define	SKIN681	1681
#define	SKIN682	1682
#define	SKIN683	1683
#define	SKIN684	1684
#define	SKIN685	1685
#define	SKIN686	1686
#define	SKIN687	1687
#define	SKIN688	1688
#define	SKIN689	1689
#define	SKIN690	1690
#define	SKIN691	1691
#define	SKIN692	1692
#define	SKIN693	1693
#define	SKIN694	1694
#define	SKIN695	1695
#define	SKIN696	1696
#define	SKIN697	1697
#define	SKIN698	1698
#define	SKIN699	1699
#define	SKIN700	1700
#define	SKIN701	1701
#define	SKIN702	1702
#define	SKIN703	1703
#define	SKIN704	1704
#define	SKIN705	1705
#define	SKIN706	1706
#define	SKIN707	1707
#define	SKIN708	1708
#define	SKIN709	1709
#define	SKIN710	1710
#define	SKIN711	1711
#define	SKIN712	1712
#define	SKIN713	1713
#define	SKIN714	1714
#define	SKIN715	1715
#define	SKIN716	1716
#define	SKIN717	1717
#define	SKIN718	1718
#define	SKIN719	1719
#define	SKIN720	1720
#define	SKIN721	1721
#define	SKIN722	1722
#define	SKIN723	1723
#define	SKIN724	1724
#define	SKIN725	1725
#define	SKIN726	1726
#define	SKIN727	1727
#define	SKIN728	1728
#define	SKIN729	1729
#define	SKIN730	1730
#define	SKIN731	1731
#define	SKIN732	1732
#define	SKIN733	1733
#define	SKIN734	1734
#define	SKIN735	1735
#define	SKIN736	1736
#define	SKIN737	1737
#define	SKIN738	1738
#define	SKIN739	1739
#define	SKIN740	1740
#define	SKIN741	1741
#define	SKIN742	1742
#define	SKIN743	1743
#define	SKIN744	1744
#define	SKIN745	1745
#define	SKIN746	1746
#define	SKIN747	1747
#define	SKIN748	1748
#define	SKIN749	1749
#define	SKIN750	1750
#define	SKIN751	1751
#define	SKIN752	1752
#define	SKIN753	1753
#define	SKIN754	1754
#define	SKIN755	1755
#define	SKIN756	1756
#define	SKIN757	1757
#define	SKIN758	1758
#define	SKIN759	1759
#define	SKIN760	1760
#define	SKIN761	1761
#define	SKIN762	1762
#define	SKIN763	1763
#define	SKIN764	1764
#define	SKIN765	1765
#define	SKIN766	1766
#define	SKIN767	1767
#define	SKIN768	1768
#define	SKIN769	1769
#define	SKIN770	1770
#define	SKIN771	1771
#define	SKIN772	1772
#define	SKIN773	1773
#define	SKIN774	1774
#define	SKIN775	1775
#define	SKIN776	1776
#define	SKIN777	1777
#define	SKIN778	1778
#define	SKIN779	1779
#define	SKIN780	1780
#define	SKIN781	1781
#define	SKIN782	1782
#define	SKIN783	1783
#define	SKIN784	1784
#define	SKIN785	1785
#define	SKIN786	1786
#define	SKIN787	1787
#define	SKIN788	1788
#define	SKIN789	1789
#define	SKIN790	1790
#define	SKIN791	1791
#define	SKIN792	1792
#define	SKIN793	1793
#define	SKIN794	1794
#define	SKIN795	1795
#define	SKIN796	1796
#define	SKIN797	1797
#define	SKIN798	1798
#define	SKIN799	1799
#define	SKIN800	1800
#define	SKIN801	1801
#define	SKIN802	1802
#define	SKIN803	1803
#define	SKIN804	1804
#define	SKIN805	1805
#define	SKIN806	1806
#define	SKIN807	1807
#define	SKIN808	1808
#define	SKIN809	1809
#define	SKIN810	1810
#define	SKIN811	1811
#define	SKIN812	1812
#define	SKIN813	1813
#define	SKIN814	1814
#define	SKIN815	1815
#define	SKIN816	1816
#define	SKIN817	1817
#define	SKIN818	1818
#define	SKIN819	1819
#define	SKIN820	1820
#define	SKIN821	1821
#define	SKIN822	1822
#define	SKIN823	1823
#define	SKIN824	1824
#define	SKIN825	1825
#define	SKIN826	1826
#define	SKIN827	1827
#define	SKIN828	1828
#define	SKIN829	1829
#define	SKIN830	1830
#define	SKIN831	1831
#define	SKIN832	1832
#define	SKIN833	1833
#define	SKIN834	1834
#define	SKIN835	1835
#define	SKIN836	1836
#define	SKIN837	1837
#define	SKIN838	1838
#define	SKIN839	1839
#define	SKIN840	1840
#define	SKIN841	1841
#define	SKIN842	1842
#define	SKIN843	1843
#define	SKIN844	1844
#define	SKIN845	1845
#define	SKIN846	1846
#define	SKIN847	1847
#define	SKIN848	1848
#define	SKIN849	1849
#define	SKIN850	1850
#define	SKIN851	1851
#define	SKIN852	1852
#define	SKIN853	1853
#define	SKIN854	1854
#define	SKIN855	1855
#define	SKIN856	1856
#define	SKIN857	1857
#define	SKIN858	1858
#define	SKIN859	1859
#define	SKIN860	1860
#define	SKIN861	1861
#define	SKIN862	1862
#define	SKIN863	1863
#define	SKIN864	1864
#define	SKIN865	1865
#define	SKIN866	1866
#define	SKIN867	1867
#define	SKIN868	1868
#define	SKIN869	1869
#define	SKIN870	1870
#define	SKIN871	1871
#define	SKIN872	1872
#define	SKIN873	1873
#define	SKIN874	1874
#define	SKIN875	1875
#define	SKIN876	1876
#define	SKIN877	1877
#define	SKIN878	1878
#define	SKIN879	1879
#define	SKIN880	1880
#define	SKIN881	1881
#define	SKIN882	1882
#define	SKIN883	1883
#define	SKIN884	1884
#define	SKIN885	1885
#define	SKIN886	1886
#define	SKIN887	1887
#define	SKIN888	1888
#define	SKIN889	1889
#define	SKIN890	1890
#define	SKIN891	1891
#define	SKIN892	1892
#define	SKIN893	1893
#define	SKIN894	1894
#define	SKIN895	1895
#define	SKIN896	1896
#define	SKIN897	1897
#define	SKIN898	1898
#define	SKIN899	1899
#define	SKIN900	1900
#define	SKIN901	1901
#define	SKIN902	1902
#define	SKIN903	1903
#define	SKIN904	1904
#define	SKIN905	1905
#define	SKIN906	1906
#define	SKIN907	1907
#define	SKIN908	1908
#define	SKIN909	1909
#define	SKIN910	1910
#define	SKIN911	1911
#define	SKIN912	1912
#define	SKIN913	1913
#define	SKIN914	1914
#define	SKIN915	1915
#define	SKIN916	1916
#define	SKIN917	1917
#define	SKIN918	1918
#define	SKIN919	1919
#define	SKIN920	1920
#define	SKIN921	1921
#define	SKIN922	1922
#define	SKIN923	1923
#define	SKIN924	1924
#define	SKIN925	1925
#define	SKIN926	1926
#define	SKIN927	1927
#define	SKIN928	1928
#define	SKIN929	1929
#define	SKIN930	1930
#define	SKIN931	1931
#define	SKIN932	1932
#define	SKIN933	1933
#define	SKIN934	1934
#define	SKIN935	1935
#define	SKIN936	1936
#define	SKIN937	1937
#define	SKIN938	1938
#define	SKIN939	1939
#define	SKIN940	1940
#define	SKIN941	1941
#define	SKIN942	1942
#define	SKIN943	1943
#define	SKIN944	1944
#define	SKIN945	1945
#define	SKIN946	1946
#define	SKIN947	1947
#define	SKIN948	1948
#define	SKIN949	1949
#define	SKIN950	1950
#define	SKIN951	1951
#define	SKIN952	1952
#define	SKIN953	1953
#define	SKIN954	1954
#define	SKIN955	1955
#define	SKIN956	1956
#define	SKIN957	1957
#define	SKIN958	1958
#define	SKIN959	1959
#define	SKIN960	1960
#define	SKIN961	1961
#define	SKIN962	1962
#define	SKIN963	1963
#define	SKIN964	1964
#define	SKIN965	1965
#define	SKIN966	1966
#define	SKIN967	1967
#define	SKIN968	1968
#define	SKIN969	1969
#define	SKIN970	1970
#define	SKIN971	1971
#define	SKIN972	1972
#define	SKIN973	1973
#define	SKIN974	1974
#define	SKIN975	1975
#define	SKIN976	1976
#define	SKIN977	1977
#define	SKIN978	1978
#define	SKIN979	1979
#define	SKIN980	1980
#define	SKIN981	1981
#define	SKIN982	1982
#define	SKIN983	1983
#define	SKIN984	1984
#define	SKIN985	1985
#define	SKIN986	1986
#define	SKIN987	1987
#define	SKIN988	1988
#define	SKIN989	1989
#define	SKIN990	1990
#define	SKIN991	1991
#define	SKIN992	1992
#define	SKIN993	1993
#define	SKIN994	1994
#define	SKIN995	1995
#define	SKIN996	1996
#define	SKIN997	1997
#define	SKIN998	1998
#define	SKIN999	1999
