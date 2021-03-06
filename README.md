# Haze
壁紙変更プログラム 「霞（カスミ）」 Ver 1.04 for Windows98

## はじめに
「霞（カスミ）」は複数の画像をリストに登録して、壁紙を変更するプログラムです。<br>
動作確認は Windows98 日本語版でしか行っていません。<br>
単体ではビットマップしか使用できませんが、Susie Plug-inを使用することで様々なフォーマットを扱うことができます。

## インストール
アーカイブには以下のファイルが含まれています。

- Haze.exe
  - HzEditer.exe
  - ReadMe.txt
  - history.txt

インストーラは含まれていません。<br>
これらのファイルを適当なフォルダにコピーして下さい。<br>
また、ショートカットやスタートアップ等は手動でお願いします。

バージョンアップの詳細に関してはhistory.txtを参照して下さい。

## アンインストール
フォルダごと削除して下さい。<br>
ショートカットなどを作った場合は、それらも削除して下さい。<br>
一時的に削除する場合は、設定ファイル（Haze.ini）のバックアップを取っておけば良いでしょう。

## 使い方
### Haze.exe
壁紙の変更を行うメインプログラムです。<br>
エクスプローラ等から起動すると、タスクトレイに常駐します。<br>
右または左クリックで以下のメニューが表示されます。

|メニュー|動作|
|------|----|
|キャンセル|何もしません。<br>メニューを閉じるだけです。|
|リストの編集|HzEditer.exe を起動し、リストの編集を行います。|
|オプション設定|オプションの設定を行います。|
|いますぐ壁紙を変更|壁紙を変更します。|
|バージョン情報|Haze.exe のバージョンを表示します。|
|サスペンド|壁紙の変更を行わないようにします。|
|終了|Haze.exe を終了します。|

サスペンド、壁紙変更中はトレイアイコンが変わります。

### HzEditer.exe
リストの編集、オプション設定などを行うプログラムです。<br>
４つのウィンドウで構成され、それぞれ

- 登録されている画像ファイルの一覧
- 選択項目のプレビュー
- リストに関する設定
- ログビューアー

です。

メニューの簡単な動作を紹介します。

<table>
<tr>
  <th>メニュー</th>
  <th>サブメニュー</th>
  <th>動作</th>
</tr>
<tr>
  <td rowspan="7">ファイル</td>
  <td>新規作成</td>
  <td>新規にリストを作成します。</td>
</tr>
<tr>
  <td>開く</td>
  <td>既存のリストを開きます。</td>
</tr>
<tr>
  <td>上書き保存</td>
  <td>リストを保存します。</td>
</tr>
<tr>
  <td>名前を付けて保存</td>
  <td>作業中のリストを新しい名前で保存します。</td>
</tr>
<tr>
  <td>結合</td>
  <td>現在編集中のリストに、既存のリストを追加します。</td>
</tr>
<tr>
  <td>最近使ったリスト</td>
  <td>過去に開いたリスト（最大５つ）を選択して開きます。</td>
</tr>
<tr>
  <td>終了</td>
  <td>HzEdit.exe を終了します。</td>
</tr>
<tr>
  <td rowspan="6">編集</td>
  <td>画像を追加</td>
  <td>リストに画像を追加します。</td>
</tr>
<tr>
  <td>削除</td>
  <td>選択項目をリストから削除します。</td>
</tr>
<tr>
  <td>全て選択</td>
  <td>リストの項目を全て選択します。</td>
</tr>
<tr>
  <td>選択の切り替え</td>
  <td>選択／非選択を反転します。</td>
</tr>
<tr>
  <td>移動</td>
  <td>選択項目の順序を変更します。</td>
</tr>
<tr>
  <td>プロパティ</td>
  <td>選択項目に特有の情報を表示・設定します。</td>
</tr>
<tr>
  <td rowspan="5">表示</td>
  <td>ステータスバー</td>
  <td>ステータスバーの表示／非表示を切り替えます。</td>
</tr>
<tr>
  <td>カラムの最適化</td>
  <td>リストビューの幅をウィンドウサイズに合わせます。</td>
</tr>
<tr>
  <td>ログをクリア</td>
  <td>ログをクリアします。</td>
</tr>
<tr>
  <td>最新の情報に更新</td>
  <td>プレビューを作り直したりします。</td>
</tr>
<tr>
  <td>オプション</td>
  <td>壁紙やプラグインに関するオプションを表示・設定します。</td>
</tr>
</table>

また、リストビューで右クリックすると、ポップアップメニューが表示されます。

|メニュー|動作|
|-------|---|
|プロパティ|選択項目に特有の情報を表示・設定します。|
|次回開始位置に指定|フォーカスのある項目を次回開始位置に指定します。|

Haze.exe は HzEditer.exe で最後に編集したリストを参照します。<br>
プロパティでの設定は、オプションでの設定よりも優先されます。

### 現在確認されているバグというか仕様というか
特にありません

### その他注意事項
Ver 1.00（β含む）以前とは互換性がありません。

不正な処理などが発生した場合、使っていないプラグインを外してみてください。<br>
解消される場合があります。

### 今後の予定
暇を見て機能拡張するつもりでいます。<br>
早急に付けて欲しい機能があれば、お知らせ下さい。<br>
出来る範囲内であれば優先いたします。

### 何となく言いたいこと
アイコン右クリック -> プロパティ -> バージョン情報

### 最後に
転載・配布等は御自由にどうぞ。

Haze.exe のアイコンは、うまい氏に作成していただきました。<br>
この場を借りてお礼申し上げます。

Susie を作成・公開してくださった、たけちん氏と各種プラグインの作者様に感謝します。

---
$Revision: 1.6 $

$Date: 1999/10/07 05:20:00 $
