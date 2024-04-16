$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$

fontdata	中国語フォントデータ

                      Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.


cxbig15g.dot:	15dot 繁体字フォントデータ
cxgb15g.dot:	15dot 簡体字フォントデータ
hlf15g.dot:	8dot 半角フォントデータ

＜サンプルの解説＞
	中国語フォントデータです。
	15dot と 8dot があり、15dot には繁体字と簡体字の 2 種類があります。

	データは、基本的に JIS 配列で格納されています。
	格納されているデータのフォーマットは、内蔵フォントと同じです。
	詳しくは、ドキュメントおよび sample\kanji\fontdata\format_j.txt を
	ご参照ください。

	また、データに含まれているすべてのフォントを表示するビューアプログラム
	が、sample\kanji\chinese\viewer にありますので、そちらもあわせて
	ご参照ください。


codetbl.txt:	Shift-JIS コード、CX(GB) コード対応表

＜サンプルの解説＞
	Shift-JIS コードと CX(GB) コードの対応表です。
	以下のようなデータが、フォントデータに格納されている順番にならんで
	います。

	亜  4939  889f

	左から、文字、CX(GB) コード、Shift-JIS コードとなっています。
	Shift-JIS コードに対応する漢字が存在しないものは、「・」としてあらわ
	してあります。また、対応する Shift-JIS コードがない、あるいは、対応する
	CX(GB) コードがない場合には、コードのところが、「----」となっています。

