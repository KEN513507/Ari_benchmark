# Ant Nation Domain Contract — v1.0 要件定義

以下は200語の Ubiquitous Language v1.0 **のみ**で再構成した要件定義である。外部語は接続詞・数詞・助詞のみ。

---

## I. 最上位契約

**200. Ant Nation Domain Contract** は、Bounded Context、Invariant、Cross-Context Event、性能目標、Scope制約を束ね、仕様・C++・テストの共通基準とする最上位契約である。

**1. Ant Nation** は、Ant Individual、Nest、Surface Ecology、Hazard、Observation、Intervention を包含する最上位ドメインである。

**14. Ubiquitous Language** は、仕様・C++・テスト・会話で同一意味を持つ正式語彙である。

**15. Invariant** は、実行中に必ず守るゲーム世界の制約である。

---

## II. Bounded Context 構成

**136. Bounded Context** は、モデル・用語・Invariant を独立させるDDD境界である。**137. Context Map** は、Context間の関係を定義する。

v0.1 Vertical Slice で稼働する Context は次の3つ。

- **4. Individual Context** — Ant Individual の Position、Ant Motion、Ant State、掘削・搬送を所有する。
- **5. Nest Context** — Nest Grid、Cell、Nest Entrance、Tunnel、Chamber、Dig Frontier を所有する。
- **12. Persistence Context** — Save、Load、Event Log、Snapshot を所有する。

v0.2 Expansion で追加する Context は次の6つ。

- **6. Lifecycle Context** — 卵、幼虫、蛹、成虫、老化、死亡。
- **7. Colony Context** — 社会的需要、役割配分、女王、資源。
- **8. Surface Ecology Context** — 餌、地表フェロモン、天敵、地表行動。
- **9. Hazard Context** — 雨、水害、崩落。
- **10. Intervention Context** — プレイヤー介入。
- **11. Observation Context** — カメラ、追跡、表示、統計UI。

**13. Runtime Infrastructure** は、スレッド、SIMD、GPU、メモリ等の非ドメイン技術領域であり、Bounded Context の外にある。

**139. No Cross-Context Reference** — 別 Context の Object Pointer を保持しない。**140. Intra-Context Reference** は同一 Context 内でのみ許可する。

---

## III. Scope

**2. v0.1 Vertical Slice** — Ant Individual が地下を掘り、土を運び、Nest を形成する最小完成版。

**3. v0.2 Expansion** — Lifecycle、Colony、Surface Ecology、Hazard、Intervention、Observation を追加する段階。

**195. No Collapse in v0.1** — v0.1 では Collapse Simulation を実装しない Scope 制約。

**195** により、v0.1 の Nest Context に崩落処理は存在しない。

---

## IV. 世界構造

**52. Two-Millimeter Cell Scale** — v0.1 では1 Cell = 2mm とする空間規格。

**53. World Width** = 2048 Cell。**54. World Depth** = 1024 Cell。

**47. Nest** は Nest Entrance から掘削によって成長する地下構造である。**48. Nest Grid** は地下世界を表す2D Cell 配列である。

**49. Cell** は地下世界の最小空間単位。**50. Cell Coordinate** は Grid 上の整数座標。**51. Cell Size** は現実スケールに対する1 Cell の大きさ。

**59. Cell Material** は Soil、Air、Rock、Bedrock のセル分類である。

- **55. Soil** — 通常掘削可能な地形 Material。
- **56. Air** — Worker が通行できる掘削済み Cell。
- **57. Rock** — 通常掘削不可能な Cell。
- **58. Bedrock** — World Boundary として絶対に掘れない Cell。

**46. Subterranean World** は Nest シミュレーションが存在する完全断面空間である。

---

## V. Ant Individual

**16. Ant Individual** は一匹の蟻を表す Entity。**17. Ant ID** はセッション中に Ant Individual を一意識別する ID。

**18. Worker Ant** は v0.1 で存在する唯一の蟻カーストである。

**20. Ant Motion** は Position、Heading、Speed、Turn Rate 等をまとめた値である。

- **21. Position** — Ant Individual の連続2D座標。
- **22. Surface Position** — XZ平面上として解釈する Position。
- **23. Nest Position** — XY断面上として解釈する Position。
- **24. Heading** — 現在進行方向をラジアンで表す値。
- **25. Speed** — 移動速度。
- **26. Turn Rate** — 単位時間あたりに変更できる Heading 上限。
- **27. Collision Radius** — シミュレーション上の簡易的な Ant Individual の大きさ。

**28. Forward Direction** は Heading から算出される正規化ベクトル。**29. Desired Direction** はナビゲーション層から移動層へ渡される方向。**30. Steering** は Desired Direction へ Heading を徐々に近づける処理。**31. Turn Constraint** は一 tick で急激に方向転換することを防ぐ規則。

**32. Individual Variation** は Speed、Turn Rate 等へ個体ごとの差を与える仕組みである。

**33. Gait Phase** は脚アニメーション周期内の位置。**34. Tripod Gait** は3本ずつの脚群を交互に動かす歩行方式。**35. Gait Speed Factor** は実移動速度と脚周期を結びつける係数。

**19. Ant State** は Worker Ant が現在何をしているかを示す有限状態である。**45. Ant State Machine** は Worker Ant の状態遷移を管理する FSM である。

---

## VI. Ant State 集合（v0.1）

v0.1 の Worker Ant が取り得る Ant State は次の8つ。

- **36. Idle** — 移動せず一時停止している。
- **37. Find Dig Site** — 掘るべき Dig Frontier を選択している。
- **38. Move To Dig Site** — 選択した掘削地点へ向かっている。
- **39. Dig** — Soil Cell を削っている。
- **40. Carry Soil** — 掘り出した土を保持して Nest Entrance へ移動している。
- **41. Return To Entrance** — 土を持って唯一の Nest Entrance へ戻る。
- **42. Dump Soil** — 地上へ土を置く。
- **43. Rest** — 一定時間行動を停止する。

**44. Load Speed Penalty** は Carry Soil 中の Worker Ant を減速させる規則である。

---

## VII. ナビゲーション

**91. Distance Field** は各 Air Cell から Goal までの距離を保持するナビゲーション Field。

v0.1 で使用する Distance Field は2つ。

- **92. Entrance Distance Field** — Nest Entrance へ戻るための Distance Field。
- **93. Frontier Distance Field** — Dig Frontier へ向かうための Distance Field。

**94. BFS Distance Build** は Breadth-First Search で距離を計算する処理。**95. Gradient Descent Navigation** は距離値が小さくなる Cell へ移動する方式。

**96. Walkable Cell** は Worker Ant が通常移動可能な Air Cell。**97. Cell Entry Check** は次 Cell へ移動可能か判定する処理。**98. Cell Occupancy** は Cell が Ant Individual によって使用されている状態。**99. Neighbor Query** は周囲 Cell または Worker Ant を検索する処理。

**100. Wall Following** は Tunnel 壁に沿って進む補助移動。**101. Tunnel Centering** は Worker Ant を Tunnel 中央付近に保つ補正。

**102. Congestion** は多数の Worker Ant が同一通路へ集中した状態。**103. Congestion Avoidance** は混雑 Cell を選びにくくする処理。**104. Traffic Priority** は Carry Soil 中の Worker Ant を優先する規則。**105. Movement Conflict** は複数 Worker Ant が同一 Cell へ移動しようとする状態。**106. Movement Resolution** は Movement Conflict を決定論的に解消する処理。

---

## VIII. 掘削

**63. Dig Frontier** は Air Cell に隣接する掘削可能 Soil の集合。**64. Dig Candidate** は Dig Frontier の中で Ant Individual が選択可能な Cell。**65. Dig Score** は Dig Candidate の優先度である。

**65. Dig Score** は次の5つの和で構成される。

- **66. Direction Score** — 現在の掘削進行方向を維持する度合い。
- **67. Safety Score** — Wall Thickness や World Boundary 距離から求める掘削安全度。
- **68. Crowding Score** — 周囲 Worker Ant 数による Dig Candidate へのペナルティ。
- **69. Random Score** — Nest 形状に小さな非対称性を加える値。

**60. Soil Strength** は Air へ変換するまでに必要な掘削量。**61. Dig Power** は Worker Ant が一回の Dig で減少させる Soil Strength。**62. Dig Interval** は Worker Ant が次の Dig を行えるまでの時間。

**70. Dig Mode** は Tunnel または Chamber を表す掘削方式である。

- **71. Tunnel** — 細長い移動空間。**72. Tunnel Excavation** — 前方優先で通路を延長する掘削。
- **73. Chamber** — 通路より広い地下空間。**74. Chamber Excavation** — 中心周囲を放射状に拡張する掘削。

**75. Chamber Center** は Chamber 拡張の基準点。**76. Target Chamber Radius** は Chamber 完成条件となる大きさ。

**77. Branch** は Tunnel から派生する新しい Tunnel。**78. Branch Probability** は Branch を発生させる確率。**79. Branch Cooldown** は分岐同士の最小距離制約。**80. Junction** は複数 Tunnel が接続する地点。**81. Dead End** は Nest Entrance 以外で終了する Tunnel。**82. Dead-End Requirement** は Nest 内に最低1個の Dead End を要求する Invariant。

**83. Nest Entrance** は地表と地下を接続する唯一の地点。**84. Single Entrance Constraint** は Nest Entrance 数を常時1に固定する Invariant。**85. World Boundary** は Nest Grid の外縁。**86. No External Tunnel Constraint** は World Boundary 外へ Air Cell が接続しない Invariant。**87. Nest Connectivity** は全 Air Cell が Nest Entrance から到達可能である性質。**88. Isolated Air Pocket** は Nest Entrance から到達できない禁止 Air 領域。**89. Wall Thickness** は Tunnel または Chamber 同士の間に残す Soil 厚。

**90. Nest Validator** は Nest Entrance、Nest Connectivity、World Boundary、Dead End を検証する Domain Service である。

---

## IX. Simulation Clock

**107. Fixed Tick** は Simulation Logic を一定 dt で更新する方式。**108. Simulation Tick** は世界更新回数を表す整数時刻。**109. Fixed Dt** は1 tick の固定時間長。**110. Simulation Clock** は Simulation Tick と Fixed Dt を管理する Service。

**156. Simulation Seed** は乱数系列を決める64bit値。**157. Deterministic RNG** は同一 Seed から同一系列を生成する RNG。**158. RNG Stream** は用途ごとに分離した決定論的乱数列。**159. Determinism** は同じ Seed と入力で同じ結果になる性質。

**171. SoA Layout** は x[]、y[]、state[] 等を別配列に格納する大量個体向け構造。**172. Spatial Hash** は Neighbor Query を高速化する空間インデックス。**173. Uniform Grid** は空間を一定サイズに分割する Neighbor Query 方式。**174. Throttled Update** は低優先処理を毎 tick 実行しない最適化。**175. Dirty Flag** は変更された対象だけ再計算する印。**176. Incremental Update** は全体ではなく変化部分のみ更新する方式。

---

## X. Cross-Context Event

**138. Cross-Context Event** は Context 境界を越えて伝える事実。**141. Event Bus** は Cross-Context Event を運搬する Infrastructure。**142. Event Queue** は publish された Event を Dispatch まで保持する Queue。

**143. Event Publication** は Context が Event を Queue へ追加する操作。**144. Event Subscription** は Context が関心ある Event を受信する契約。**145. Deferred Dispatch** は publish 直後に Subscriber を呼ばない規則。**146. Tick Ordering** は Event を Simulation Tick 昇順に処理する規則。**147. Sequence Ordering** は同一 Simulation Tick の Event を sequence 昇順で処理する規則。

**148. Event Header** は Simulation Tick、Sequence、Producer、Type、Version を持つ共通部。**149. Event Payload** は ID と Value だけを保持する Event データ。**150. Immutable Event** は発行後の Event 内容を変更しない原則。**155. Event Schema Version** は Event 構造の互換性管理番号。

v0.1 で定義する Event は次の3つ。

- **153. SoilCellExcavated** — Individual Context から Nest Context へ掘削完了を通知する。
- **152. DigTaskAvailable** — Nest Context から Individual Context へ掘削可能作業を通知する。
- （v0.1 で Cross-Context Event は上記2つに限定する。）

v0.2 で追加する Event は次の2つ。

- **151. SpaceDemandChanged** — Colony Context から Nest Context へ空間需要を通知する。
- **154. ChamberExpanded** — Nest Context から Colony Context へ空間増加を通知する。

---

## XI. Persistence

**160. Event Log** は発生した Cross-Context Event を順序付きで保存する永続データ。**161. Event Sourcing** は永続化上の真実の源を Event Log とする方式。**162. Snapshot** は高速 Load 用に保存する任意の World State キャッシュ。

**163. Save File** は Event Log と Snapshot 等を格納するファイル。**164. Save Version** は Save 形式のバージョン番号。**165. Created At** は Save 作成時刻。**166. Saved Simulation Tick** は Save 時点の Simulation Tick。

**167. Load** は Event Log と Snapshot から現在状態を復元する処理。**168. Save Migration** は古い Save Version を新形式へ変換する処理。**169. Deterministic Bug Reproduction** は Simulation Seed と Event Log を使い不具合条件を再現する技術用途。

**170. No World Rewind** — 過去 World State へ戻す製品機能を禁止する Invariant。

---

## XII. 描画

**111. Ant Mesh** は現在の392 triangle完全蟻モデルである。

**112〜116 LOD Ant 集合** —

- **112. LOD0 Ant** — 0〜50mm で表示する392 triangle完全モデル。
- **113. LOD1 Ant** — 50〜200mm で表示する約120 triangle モデル。
- **114. LOD2 Ant** — 200〜800mm で表示する3球体モデル。
- **115. LOD3 Ant** — 800mm〜3m で表示する単一三角形モデル。
- **116. LOD4 Population** — 3m 以遠または画面外で使用する点・密度表現。

**117. LOD Selection** はカメラ距離によって LOD を選択する処理。

**118. Ant Render Instance** は GPU へ渡す Ant Individual の描画データ。**119. Ant Render Bridge** は Individual Context から Ant Render Instance へ変換する Adapter。**120. OpenGL Instancing** は1つの Ant Mesh を多数描画する GPU 方式。

**121. Flat Color** はライティングを使わず固定色で描く表現方式。**122. Single-Color Worker Rule** は Worker Ant 一個体に複数色を使わない Visual Invariant。

---

## XIII. 観察

**123. Surface View** は XZ平面を真上から見るモード。**124. Nest View** は XY断面を横から見るモード。

**125. SURFACE_GLOBAL** は地表全体を観察する標準カメラ状態。**126. NEST_GLOBAL** は Nest 全体断面を観察するカメラ状態。**127. Follow Individual** は指定した Ant ID へカメラが追従する状態。**128. FOLLOW_SURFACE** は地上での個体追跡モード。**129. FOLLOW_NEST** は地下での個体追跡モード。**130. Camera Transition** は Worker Ant が Nest Entrance を通過した際に Surface View と Nest View を補間する状態。

**131. Follow Release** は ESC で Follow Individual を終了する操作。**132. Follow Tab Lock** は Follow Individual 中は手動 View 切替を禁止する規則。**133. Death Follow Release** は追跡個体死亡時に GLOBAL へ戻る規則。**134. Camera Control** はプレイヤーに許可された視点操作。**135. Marking** は Ant Individual へ印を付ける正式 DDD 概念。

---

## XIV. 予算

**177. CPU Frame Budget** は1 frame 内に許可する CPU 処理時間。60FPS 時の内訳は次の通り。

- **178. Simulation Logic Budget** = 4.0ms
- **179. Event Dispatch Budget** = 0.5ms
- **180. Rendering Prep Budget** = 2.0ms
- **181. GPU Submit Budget** = 1.5ms
- **182. OS/SDL Budget** = 1.0ms
- **183. Safety Margin** = 7.6ms（50% 余裕）

**186. Local Development Target** — GTX 970 で100匹および10,000匹 @ 60FPS を検証する目標。**185. Low-End Target** — GTX 1050 で1,000匹 @ 30FPS を保証する低ライン。**184. 24GB Stress Target** — Vast.ai 上で1,000,000匹 @ 12FPS を目標とする性能契約。

**187. Statistical Population** は Full Simulation が予算超過した場合の集団近似表現。**188. Statistical Fallback Threshold** は100万匹時 Simulation Logic が30ms を超えた場合の切替条件。

---

## XV. 完成条件

**194. v0.1 Completion Gate** は次の5テストすべての同時 PASS を要求する Gate。

- **189. Determinism Test** — 同 Simulation Seed・同入力を100回実行し Event Log 一致を確認する。
- **190. Connectivity Invariant Test** — 1000 Simulation Tick 後の全 Air Cell が Nest Entrance から BFS 到達可能か確認する。
- **191. Single Entrance Test** — 実行中 Nest Entrance 数が常に1か検証する。
- **192. Frame Budget Test** — GTX970 で100匹・10,000匹 @ 60FPS を確認する。
- **193. No External Tunnel Test** — 全 Tunnel が World Boundary 内にあることを確認する。

---

## XVI. 禁止事項（Product Invariant）

- **196. No Direct Ant Command** — プレイヤーが Ant Individual へ個別命令することを禁止する。
- **197. No Win-Loss Objective** — 勝利条件、敗北条件、達成目標を提示しない。
- **170. No World Rewind** — 過去 World State へ戻す製品機能を禁止する。
- **195. No Collapse in v0.1** — v0.1 では Collapse Simulation を実装しない。
- **198. Allowed Intervention Set** — v0.2 以降で許可される介入を7種類だけに固定する Policy。
- **199. Seven Allowed Interventions** — Food Placement、Water Pouring、Hole Blocking、Poking、Marking、Time Control、Camera Control の固定集合。

---

## XVII. 契約の要約

**200. Ant Nation Domain Contract** が保証するのは次の7点である。

1. **v0.1 Vertical Slice** は Individual Context、Nest Context、Persistence Context の3つで成立する。
2. **Two-Millimeter Cell Scale** と **World Width 2048 × World Depth 1024** が空間規格である。
3. **Ant State Machine** は Idle、Find Dig Site、Move To Dig Site、Dig、Carry Soil、Return To Entrance、Dump Soil、Rest の8状態を持つ。
4. **Dig Score** は Direction Score、Safety Score、Crowding Score、Random Score の和である。
5. **Cross-Context Event** は Event Bus を介し、Deferred Dispatch、Tick Ordering、Sequence Ordering に従う。
6. **Persistence Context** は Event Sourcing を採用し、Snapshot を補助とする。No World Rewind を守る。
7. **v0.1 Completion Gate** は5つのテスト（Determinism、Connectivity、Single Entrance、Frame Budget、No External Tunnel）が同時 PASS することを要求する。

---

## 検証

この要件定義は、200語の **14. Ubiquitous Language** の外部語を一切使わずに、次を再現した。

- Bounded Context 構成（v0.1: 3、v0.2: 6、Runtime: 1）
- 空間仕様（Two-Millimeter Cell Scale、World Width/Depth）
- Ant Individual と Ant State Machine
- Dig Frontier と Dig Score の構成
- Distance Field ナビゲーション
- Cross-Context Event 契約
- Persistence 方針
- LOD 集合と描画規則
- Camera 状態（SURFACE_GLOBAL / NEST_GLOBAL / FOLLOW_* / Camera Transition）
- CPU Frame Budget 配分
- v0.1 Completion Gate の5テスト
- 6つの禁止 Invariant

したがって、**200語の DDD Ubiquitous Language v1.0 は、本ゲームの v0.1 要件定義を自己完結的に再現できる**。