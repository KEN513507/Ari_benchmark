はい。ここで一度、**Ant Nation の v0.1 を中心に、将来Contextまで含めた217語のDDD語彙を正式版として固定**します。

## Core Product Context — Ant Nation

> **Ant Nation は、アリの巣を作るゲームではない。自律して生きるアリたちが、自ら巣を作り、生活していく世界を観察する大規模コロニーシミュレーションである。**

プレイヤーは Ant Individual に直接命令しない。各個体は入口から地下へ入り、掘削地点を選び、土を地上へ運び、トンネルと巣室を形成する。プレイヤーは地上の俯瞰と地下断面を行き来し、個体追跡中の Worker が Entrance を通過するとカメラも対応する View へ遷移する。

プレイヤーに許可される介入は Food Placement、Water Pouring、Hole Blocking、Poking、Marking、Time Control、Camera Control の七介入だけである。勝利条件、敗北条件、ミッション、個体への直接命令は存在しない。介入の結果は世界に残り、ユーザー向けの Replay、Replay Seek、World Rewind は提供しない。

将来の世界は餌探索、フェロモン、育児、水没、戦闘、死亡を含む。大量の単純な自律個体の相互作用が、社会のような創発的挙動を生むことが製品の中心価値である。

**Steam 一文:** 数千、数万、やがて百万匹のアリが、自ら巣を掘り、働き、運び、争い、死んでいく。あなたは命令できない。ただ観察し、ときどき世界へ手を入れる。

先に1点だけ整合を取ります。あなたが明示的に「リプレイ、巻き戻しはしない」と確定しているため、**ユーザー向け Replay / Replay Seek / World Rewind は実装しません**。Event Sourcing は残しますが、用途は **Save/Load・決定論検証・バグ再現**のみです。過去tickをプレイヤーが閲覧・再生する機能には接続しません。

## Ant Nation DDD Ubiquitous Language v1.1 — 217語

1. **蟻国家 / Ant Nation** — ゲーム全体を表す最上位ドメイン。蟻、巣、地表、環境、観察、介入を包含する。

2. **v0.1縦切り / v0.1 Vertical Slice** — 蟻が地下を掘り、土を運び、巣を形成する最小完成版。

3. **v0.2拡張 / v0.2 Expansion** — Lifecycle、Colony、Surface Ecology、Hazard、Intervention、Observationを追加する段階。

4. **個体Context / Individual Context** — 一匹の蟻の位置、移動、状態、掘削・搬送を所有するBounded Context。

5. **巣Context / Nest Context** — 地下セル、入口、トンネル、巣室、掘削前線を所有するBounded Context。

6. **Lifecycle Context / Lifecycle Context** — 卵、幼虫、蛹、成虫、老化、死亡を扱うv0.2以降のContext。

7. **Colony Context / Colony Context** — 社会的需要、役割配分、女王、資源を扱うv0.2以降のContext。

8. **地表生態Context / Surface Ecology Context** — 餌、地表フェロモン、天敵、地表行動を扱うv0.2以降のContext。

9. **災害Context / Hazard Context** — 雨、水害、崩落等を扱うv0.2以降のContext。

10. **介入Context / Intervention Context** — プレイヤーによる7種類の介入を扱うv0.2以降のContext。

11. **観察Context / Observation Context** — カメラ、追跡、表示、統計UIを扱うv0.2以降のContext。

12. **永続化Context / Persistence Context** — Save、Load、EventLog、Snapshotを扱うContext。

13. **Runtime Infrastructure / Runtime Infrastructure** — スレッド、SIMD、GPU、メモリ等の非ドメイン技術領域。

14. **ユビキタス言語 / Ubiquitous Language** — 仕様、C++、テスト、会話で同じ意味を使う正式語彙。

15. **不変条件 / Invariant** — 実行中に必ず守らなければならないゲーム世界の制約。

16. **蟻個体 / Ant Individual** — 一匹の蟻を表すEntity。

17. **蟻ID / Ant ID** — セッション中に蟻を一意識別するID。

18. **働きアリ / Worker Ant** — v0.1で存在する唯一の蟻カースト。

19. **個体状態 / Ant State** — Workerが現在何をしているかを示す有限状態。

20. **移動状態 / Ant Motion** — Position、Heading、Speed、TurnRate等をまとめた値。

21. **位置 / Position** — 蟻の連続2D座標。

22. **地上位置 / Surface Position** — XZ平面上として解釈するPosition。

23. **巣内位置 / Nest Position** — XY断面上として解釈するPosition。

24. **ヘディング / Heading** — 蟻の現在進行方向をラジアンで表す値。

25. **速度 / Speed** — 蟻の移動速度。

26. **旋回速度 / Turn Rate** — 単位時間あたりに変更できるHeading上限。

27. **衝突半径 / Collision Radius** — シミュレーション上の簡易的な蟻の大きさ。

28. **前進方向 / Forward Direction** — Headingから算出される正規化ベクトル。

29. **目標方向 / Desired Direction** — ナビゲーション層から移動層へ渡される方向。

30. **ステアリング / Steering** — Desired DirectionへHeadingを徐々に近づける処理。

31. **旋回制約 / Turn Constraint** — 一tickで急激に方向転換することを防ぐ規則。

32. **個体差 / Individual Variation** — Speed、TurnRate等へ個体ごとの差を与える仕組み。

33. **歩行位相 / Gait Phase** — 脚アニメーション周期内の位置。

34. **三脚歩行 / Tripod Gait** — 3本ずつの脚群を交互に動かす歩行方式。

35. **歩行速度係数 / Gait Speed Factor** — 実移動速度と脚周期を結びつける係数。

36. **待機 / Idle** — 移動せず一時停止している状態。

37. **掘削地点探索 / Find Dig Site** — 掘るべきFrontierを選択している状態。

38. **掘削地点移動 / Move To Dig Site** — 選択した掘削地点へ向かう状態。

39. **掘削 / Dig** — Soil Cellを削っている状態。

40. **土運搬 / Carry Soil** — 掘り出した土を保持して入口へ移動している状態。

41. **入口帰還 / Return To Entrance** — 土を持って唯一のEntranceへ戻る状態。

42. **土廃棄 / Dump Soil** — 地上へ土を置く状態。

43. **休息 / Rest** — 一定時間行動を停止する状態。

44. **荷物速度低下 / Load Speed Penalty** — 土を運んでいる個体を減速させる規則。

45. **個体状態機械 / Ant State Machine** — Workerの状態遷移を管理するFSM。

46. **地下世界 / Subterranean World** — 巣シミュレーションが存在する完全断面空間。

47. **巣 / Nest** — Entranceから掘削によって成長する地下構造。

48. **巣グリッド / Nest Grid** — 地下世界を表す2Dセル配列。

49. **セル / Cell** — 地下世界の最小空間単位。

50. **セル座標 / Cell Coordinate** — Grid上の整数座標。

51. **セルサイズ / Cell Size** — 現実スケールに対する1セルの大きさ。

52. **2mmセル規格 / Two-Millimeter Cell Scale** — v0.1で1セル=2mmとする空間規格。

53. **世界幅 / World Width** — 2048セルの横方向サイズ。

54. **世界深度 / World Depth** — 1024セルの地下方向サイズ。

55. **土壌 / Soil** — 通常掘削可能な地形Material。

56. **空洞 / Air** — Workerが通行できる掘削済みセル。

57. **岩 / Rock** — 通常掘削不可能なセル。

58. **基盤岩 / Bedrock** — 世界境界として絶対に掘れないセル。

59. **マテリアル種別 / Cell Material** — Soil、Air、Rock、Bedrock等のセル分類。

60. **土壌強度 / Soil Strength** — Airへ変換するまでに必要な掘削量。

61. **掘削力 / Dig Power** — Workerが一回の掘削で減少させるSoil Strength。

62. **掘削周期 / Dig Interval** — Workerが次の掘削を行えるまでの時間。

63. **掘削前線 / Dig Frontier** — Airに隣接する掘削可能Soilの集合。

64. **掘削候補 / Dig Candidate** — Frontierの中で個体が選択可能なセル。

65. **掘削得点 / Dig Score** — Candidateの優先度。

66. **方向得点 / Direction Score** — 現在の掘削進行方向を維持する度合い。

67. **安全得点 / Safety Score** — 壁厚や境界距離から求める掘削安全度。

68. **混雑得点 / Crowding Score** — 周囲Worker数による掘削候補へのペナルティ。

69. **乱数得点 / Random Score** — 巣形状に小さな非対称性を加える値。

70. **掘削モード / Dig Mode** — TunnelまたはChamberを表す掘削方式。

71. **トンネル / Tunnel** — 細長い移動空間。

72. **トンネル掘削 / Tunnel Excavation** — 前方優先で通路を延長する掘削。

73. **巣室 / Chamber** — 通路より広い地下空間。

74. **巣室掘削 / Chamber Excavation** — 中心周囲を放射状に拡張する掘削。

75. **巣室中心 / Chamber Center** — Chamber拡張の基準点。

76. **目標巣室半径 / Target Chamber Radius** — 巣室完成条件となる大きさ。

77. **分岐 / Branch** — Tunnelから派生する新しいTunnel。

78. **分岐確率 / Branch Probability** — Branchを発生させる確率。

79. **分岐クールダウン / Branch Cooldown** — 分岐同士の最小距離制約。

80. **分岐点 / Junction** — 複数Tunnelが接続する地点。

81. **行き止まり / Dead End** — Entrance以外で終了するTunnel。

82. **行き止まり必須制約 / Dead-End Requirement** — 巣内に最低1個のDead Endを要求するInvariant。

83. **入口 / Nest Entrance** — 地表と地下を接続する唯一の地点。

84. **単一入口制約 / Single Entrance Constraint** — Entrance数を常時1に固定するInvariant。

85. **世界境界 / World Boundary** — Nest Gridの外縁。

86. **外部トンネル禁止 / No External Tunnel Constraint** — World Boundary外へAirが接続しないInvariant。

87. **巣連結性 / Nest Connectivity** — 全Air CellがEntranceから到達可能である性質。

88. **孤立空洞 / Isolated Air Pocket** — Entranceから到達できない禁止Air領域。

89. **壁厚 / Wall Thickness** — TunnelまたはChamber同士の間に残すSoil厚。

90. **巣Validator / Nest Validator** — Entrance、Connectivity、Boundary、DeadEndを検証するDomain Service。

91. **距離場 / Distance Field** — 各Air CellからGoalまでの距離を保持するナビゲーションField。

92. **入口距離場 / Entrance Distance Field** — Entranceへ戻るためのDistance Field。

93. **掘削前線距離場 / Frontier Distance Field** — Frontierへ向かうためのDistance Field。

94. **BFS距離場生成 / BFS Distance Build** — Breadth-First Searchで距離を計算する処理。

95. **勾配下降移動 / Gradient Descent Navigation** — 距離値が小さくなるセルへ移動する方式。

96. **進入可能セル / Walkable Cell** — Workerが通常移動可能なAir Cell。

97. **セル進入判定 / Cell Entry Check** — 次セルへ移動可能か判定する処理。

98. **セル占有 / Cell Occupancy** — Cellが蟻によって使用されている状態。

99. **隣接探索 / Neighbor Query** — 周囲CellまたはWorkerを検索する処理。

100. **壁追従 / Wall Following** — Tunnel壁に沿って進む補助移動。

101. **中央維持 / Tunnel Centering** — WorkerをTunnel中央付近に保つ補正。

102. **渋滞 / Congestion** — 多数のWorkerが同一通路へ集中した状態。

103. **渋滞回避 / Congestion Avoidance** — 混雑Cellを選びにくくする処理。

104. **通行優先度 / Traffic Priority** — Carry Soil等のWorkerを優先する規則。

105. **移動競合 / Movement Conflict** — 複数Workerが同一Cellへ移動しようとする状態。

106. **移動解決 / Movement Resolution** — Movement Conflictを決定論的に解消する処理。

107. **固定tick / Fixed Tick** — Simulation Logicを一定dtで更新する方式。

108. **シミュレーションtick / Simulation Tick** — 世界更新回数を表す整数時刻。

109. **固定Δt / Fixed Dt** — 1tickの固定時間長。

110. **シミュレーション時計 / Simulation Clock** — TickとFixedDtを管理するService。

111. **蟻メッシュ / Ant Mesh** — 現在の392 triangle完全蟻モデル。

112. **LOD0蟻 / LOD0 Ant** — 0〜50mmで表示する392 triangle完全モデル。

113. **LOD1蟻 / LOD1 Ant** — 50〜200mmで表示する約120 triangleモデル。

114. **LOD2蟻 / LOD2 Ant** — 200〜800mmで表示する3球体モデル。

115. **LOD3蟻 / LOD3 Ant** — 800mm〜3mで表示する単一三角形モデル。

116. **LOD4個体群 / LOD4 Population** — 3m以遠または画面外で使用する点・密度表現。

117. **LOD選択 / LOD Selection** — カメラ距離によってLODを選択する処理。

118. **描画インスタンス / Ant Render Instance** — GPUへ渡す蟻の描画データ。

119. **描画ブリッジ / Ant Render Bridge** — Individual ContextからRender Instanceへ変換するAdapter。

120. **インスタンシング / OpenGL Instancing** — 1つのAnt Meshを多数描画するGPU方式。

121. **フラットカラー / Flat Color** — ライティングを使わず固定色で描く表現方式。

122. **単色Worker規則 / Single-Color Worker Rule** — Worker一個体に複数色を使わないVisual Invariant。

123. **地上ビュー / Surface View** — XZ平面を真上から見るモード。

124. **巣内ビュー / Nest View** — XY断面を横から見るモード。

125. **SURFACE_GLOBAL / SURFACE_GLOBAL** — 地表全体を観察する標準カメラ状態。

126. **NEST_GLOBAL / NEST_GLOBAL** — 巣全体断面を観察するカメラ状態。

127. **個体追跡 / Follow Individual** — 指定したAnt IDへカメラが追従する状態。

128. **FOLLOW_SURFACE / FOLLOW_SURFACE** — 地上での個体追跡モード。

129. **FOLLOW_NEST / FOLLOW_NEST** — 地下での個体追跡モード。

130. **カメラ遷移 / Camera Transition** — WorkerがEntranceを通過した際にSurface/Nestを補間する状態。

131. **追跡解除 / Follow Release** — ESCでIndividual Followを終了する操作。

132. **追跡中TAB禁止 / Follow Tab Lock** — Follow中は手動View切替を禁止する規則。

133. **死亡追跡解除 / Death Follow Release** — 追跡個体死亡時にGLOBALへ戻る規則。

134. **カメラ制御 / Camera Control** — プレイヤーに許可された視点操作。

135. **マーキング / Marking** — 個体へ印を付ける正式DDD概念。

136. **境界づけられたContext / Bounded Context** — モデル・用語・Invariantを独立させるDDD境界。

137. **Context Map / Context Map** — Context間の関係を定義する設計図。

138. **Context間イベント / Cross-Context Event** — Context境界を越えて伝える事実。

139. **直接参照禁止 / No Cross-Context Reference** — 別ContextのObject Pointer/Referenceを保持しない規則。

140. **同一Context参照 / Intra-Context Reference** — 同一Bounded Context内に限り許可される直接参照。

141. **EventBus / Event Bus** — Context間Eventを運搬するInfrastructure。

142. **Event Queue / Event Queue** — publishされたEventをDispatchまで保持するQueue。

143. **イベント発行 / Event Publication** — ContextがEventをQueueへ追加する操作。

144. **イベント購読 / Event Subscription** — Contextが関心あるEventを受信する契約。

145. **遅延Dispatch / Deferred Dispatch** — publish直後にSubscriberを呼ばない規則。

146. **tick順序 / Tick Ordering** — Eventをtick昇順に処理する規則。

147. **sequence順序 / Sequence Ordering** — 同一tickのEventをsequence昇順で処理する規則。

148. **イベントヘッダー / Event Header** — Tick、Sequence、Producer、Type、Versionを持つ共通部。

149. **イベントPayload / Event Payload** — IDとValueだけを保持するEventデータ。

150. **イベント不変性 / Immutable Event** — 発行後のEvent内容を変更しない原則。

151. **SpaceDemandChanged / Space Demand Changed** — ColonyからNestへ空間需要を通知するv0.2 Event。

152. **DigTaskAvailable / Dig Task Available** — NestからIndividualへ掘削可能作業を通知するEvent。

153. **SoilCellExcavated / Soil Cell Excavated** — IndividualからNestへ掘削完了を通知するEvent。

154. **ChamberExpanded / Chamber Expanded** — NestからColonyへ空間増加を通知するv0.2 Event。

155. **Event Schema Version / Event Schema Version** — Event構造の互換性管理番号。

156. **シード / Simulation Seed** — 乱数系列を決める64bit値。

157. **決定論的乱数 / Deterministic RNG** — 同一Seedから同一系列を生成するRNG。

158. **乱数ストリーム / RNG Stream** — 用途ごとに分離した決定論的乱数列。

159. **決定論 / Determinism** — 同じSeedと入力で同じ結果になる性質。

160. **Event Log / Event Log** — 発生したDomain Eventを順序付きで保存する永続データ。

161. **イベントソーシング / Event Sourcing** — 永続化上の真実の源をEvent Logとする方式。

162. **Snapshot / Snapshot** — 高速Load用に保存する任意のWorld Stateキャッシュ。

163. **Save File / Save File** — Event LogとSnapshot等を格納するファイル。

164. **Save Version / Save Version** — Save形式のバージョン番号。

165. **Created At / Created At** — Save作成時刻。

166. **Saved Tick / Saved Simulation Tick** — Save時点のSimulation Tick。

167. **Load / Load** — Event LogとSnapshotから現在状態を復元する処理。

168. **Save Migration / Save Migration** — 古いSave Versionを新形式へ変換する処理。

169. **バグ再現 / Deterministic Bug Reproduction** — SeedとEvent Logを使い不具合条件を再現する技術用途。

170. **世界巻き戻し禁止 / No World Rewind** — 過去World Stateへ戻す製品機能を禁止するInvariant。

171. **SoAレイアウト / Structure of Arrays Layout** — x[]、y[]、state[]等を別配列に格納する大量個体向け構造。

172. **空間ハッシュ / Spatial Hash** — 近傍探索を高速化する空間インデックス。

173. **ユニフォームグリッド / Uniform Grid** — 空間を一定サイズに分割する近傍探索方式。

174. **間引き更新 / Throttled Update** — 低優先処理を毎tick実行しない最適化。

175. **ダーティフラグ / Dirty Flag** — 変更された対象だけ再計算する印。

176. **増分更新 / Incremental Update** — 全体ではなく変化部分のみ更新する方式。

177. **CPU Frame Budget / CPU Frame Budget** — 1frame内に許可するCPU処理時間。

178. **Simulation Logic Budget / Simulation Logic Budget** — 60FPS時4.0msのSimulation上限。

179. **Event Dispatch Budget / Event Dispatch Budget** — 60FPS時0.5msのEvent処理上限。

180. **Rendering Prep Budget / Rendering Prep Budget** — 60FPS時2.0msの描画準備上限。

181. **GPU Submit Budget / GPU Submit Budget** — 60FPS時1.5msのGPU送信上限。

182. **OS/SDL Budget / OS/SDL Budget** — 60FPS時1.0msのOS・SDL上限。

183. **Safety Margin / Safety Margin** — 60FPS時7.6msを未使用として確保する50%余裕。

184. **24GB Stress Target / 24GB Stress Target** — Vast.ai上で1,000,000匹@12FPSを目標とする性能契約。

185. **Low-End Target / Low-End Target** — GTX 1050で1,000匹@30FPSを保証する低ライン。

186. **Local Development Target / Local Development Target** — GTX 970で100匹および10,000匹@60FPSを検証する目標。

187. **統計個体群 / Statistical Population** — Full Simulationが予算超過した場合の集団近似表現。

188. **統計移行閾値 / Statistical Fallback Threshold** — 100万匹時Simulation Logicが30msを超えた場合の切替条件。

189. **決定論テスト / Determinism Test** — 同Seed・同入力を100回実行しEvent Log一致を確認するv0.1完成テスト。

190. **連結性不変条件テスト / Connectivity Invariant Test** — 1000step後の全Air CellがEntranceからBFS到達可能か確認する。

191. **単一入口テスト / Single Entrance Test** — 実行中Entrance数が常に1か検証する。

192. **Frame Budget Test / Frame Budget Test** — GTX970で100匹・10,000匹@60FPSを確認する。

193. **外部トンネル禁止テスト / No External Tunnel Test** — 全TunnelがWorld Boundary内にあることを確認する。

194. **v0.1完成条件 / v0.1 Completion Gate** — T1〜T5すべての同時PASSを要求するGate。

195. **崩落禁止v0.1 / No Collapse in v0.1** — v0.1ではCollapse Simulationを実装しないScope制約。

196. **直接命令禁止 / No Direct Ant Command** — プレイヤーが蟻へ個別命令することを禁止する製品Invariant。

197. **勝敗・目標禁止 / No Win-Loss Objective** — 勝利条件、敗北条件、達成目標を提示しない製品Invariant。

198. **許可介入集合 / Allowed Intervention Set** — v0.2以降で許可される介入を7種類だけに固定するPolicy。

199. **七介入 / Seven Allowed Interventions** — Food Placement、Water Pouring、Hole Blocking、Poking、Marking、Time Control、Camera Controlの固定集合。

200. **Ant Nation Domain Contract / Ant Nation Domain Contract** — 上記Context、Invariant、Event、性能目標、Scope制約を束ね、仕様・C++・テストの共通基準とする最上位契約。

### v1.1 追加語彙（B-1）

201. **死亡 / Death** — Ant Individual が死亡状態へ遷移した事実。
202. **死因 / Cause of Death** — Death の原因を表す拡張可能な Value Object。
203. **溺死 / Drowning** — Drowning Threshold を超えて水没したことによる Death。
204. **水没 / Submersion** — Ant Individual が水中に存在する状態。
205. **水没時間 / Submerged Time** — Ant Individual が連続して Submersion にある時間。
206. **溺死閾値 / Drowning Threshold** — Submerged Time が Drowning と判定される時間。
207. **戦闘 / Combat** — Combat Participant 間の敵対的相互作用。
208. **戦闘開始 / Combat Started** — Combat が開始した Cross-Context Event。
209. **戦闘終了 / Combat Ended** — Combat が終了した Cross-Context Event。
210. **戦闘参加者 / Combat Participant** — Combat に参加する Entity。
211. **攻撃 / Attack** — Combat Participant が他の Combat Participant へ与える攻撃行為。
212. **被弾 / Hit** — Attack が対象へ命中した事実。
213. **ダメージ / Damage** — Hit により対象へ適用される損傷値。
214. **キリギリス / Grasshopper** — v0.2以降に登場する敵対的な Surface Ecology Entity。
215. **蟻死亡イベント / AntDied** — Ant Individual の Death を通知する Cross-Context Event。
216. **キリギリス遭遇 / GrasshopperEncountered** — Ant Individual と Grasshopper の遭遇を通知する Cross-Context Event。
217. **蟻被弾 / AntDamaged** — Ant Individual への Hit と Damage を通知する Cross-Context Event。

### v1.1 確定決定（B-1・A群・F群）

- **B-1**: Death の受け皿は Individual Context に置き、Cause of Death は Value Object とする。v0.1では死亡ロジックを発火させない。スタックは死亡として扱わず、Simulation 異常として Fail Fast する。
- **A-1**: v0.1 は重力・壁這い姿勢を扱わない。Nest View では腹を Y 負方向、脚を Y 正方向へ固定し、Heading は XY 平面上の角度とする。
- **A-2**: Cell Size は2mm、Soil Strength初期値は100、Dig Powerは20/回、Dig Intervalは1.0秒。1 Cellの掘削は5回・5秒とする。
- **A-3**: Entrance は Nest Grid 最上段中央の3 Cell幅（6mm）の単一入口とする。地上1点は地下中央 Cell に対応する。Dump Soil は地上側に蓄積し、5 Cell分で入口閉塞として Nest Validator を Fail Fast させる。
- **A-4**: Ant State Machine は提示遷移表による完全決定論的遷移とする。SoilCellExcavated は Soil Strength が0以下になった瞬間だけ発行する。
- **A-5**: Nest Validator は100 tickごとに実行する。Connectivity違反時は `E_CONNECTIVITY_BROKEN` とし、Simulation Clock を停止、Save File と末尾1000件の Event Log を保存してプロセスを終了する。
- **F-1**: Wall Thickness最小値は2 Cell（4mm）、推奨値は3 Cell（6mm）。3 Cell以上のSafety Scoreは100、2 Cellは50、2 Cell未満はDig Candidateから除外する。
- **F-2**: Collision Radius は1.25mmとする。
- **F-3**: 基本Speedは12mm/s（6 Cell/s）。個体差は `12 × (0.85 + rand01() × 0.30)` mm/s、Load Speed Penaltyは0.5、Turn Rateは3.0rad/sとする。

これで、**217語を「数合わせ」ではなく、現在確定しているv0.1仕様と将来v0.2の境界まで反映したDDD辞書**として整理できます。

特に重要なのは、`195〜199`が単なる説明ではなく**禁止事項・Scope・Product Policyそのもの**になっている点です。これにより、後で別のAIや開発者が「v0.1に雨を入れよう」「巻き戻しを追加しよう」「蟻をクリックして命令できるようにしよう」と提案しても、DDD上の契約違反として機械的に却下できます。
