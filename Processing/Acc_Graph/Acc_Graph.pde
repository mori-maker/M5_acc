import processing.serial.*;

Serial myserial;

float[] RollData = new float[300];
float[] PitchData = new float[300];
float[] kal_RollData = new float[300];
float[] kal_PitchData = new float[300];


//データ格納用クラス
class SensorData
{
  float Pitch, Roll,kal_Pitch, kal_Roll;
  
  //constructor
  SensorData()
  {
    Pitch = Roll = kal_Pitch = kal_Roll = 0.0;
  }
  
  void setData(float[] data)
  {
    if (data.length >6)
    {
      return;
    }
    
    Roll = data[0];
    Pitch = data[1];
    kal_Roll = data[2];
    kal_Pitch = data[3];
  }
};


SensorData s_data = new SensorData();


void setup()
{ 
  //シリアルポート確認＆指定
  printArray(Serial.list());
  myserial = new Serial(this, Serial.list()[5], 115200);  // Arduino
  
  //画面サイズ指定
  size(720,720);
  smooth();
  background(0);
  frameRate(10);
  
  //plotdata初期化
  for(int i=0; i<300; i++)
  {
    RollData[i] = 0;
    PitchData[i] =0;
    kal_RollData[i] = 0;
    kal_PitchData[i] =0;
  }
  

}


void draw()
{
  background(0);
  DataPlot(width/4, height/2-height/2/2, "Angle[deg]");
  
  print(s_data.Roll);
  print("\t");
  print(s_data.Pitch);
  print("\t");
  print(s_data.kal_Roll);
  print("\t");
  println(s_data.kal_Pitch);

}


//p_X, p_Y グラフ左上の位置

void DataPlot(int p_X, int p_Y, String name)
{
  
  float Width = width/1.5;  //グラフのサイズ
  float Height = height/1.5;  //グラフのサイズ
  
  graphInit(Width, Height, p_X, p_Y, name);

  translate(0, Height/2); //グラフ原点（X=0, Y=0） 

  //データ更新
  RollData[299] = s_data.Roll;
  PitchData[299] = s_data.Pitch;  
  kal_RollData[299] = s_data.kal_Roll;
  kal_PitchData[299] = s_data.kal_Pitch;  
  
  for(int i=0; i<300-1; i++)
  {
    RollData[i] = RollData[i+1];
    PitchData[i] = PitchData[i+1];
    kal_RollData[i] = kal_RollData[i+1];
    kal_PitchData[i] = kal_PitchData[i+1];  
  }
  
  //グラフ描写  
  for(int i=0; i<300-1; i++)
  {
    //stroke(255,255,0);
    //line(Width/300*i, Height/6*RollData[i]/30, Width/300*(i+1), Height/6*RollData[i+1]/30);

    stroke(0,255,0);
    line(Width/300*i, Height/6*PitchData[i]/30, Width/300*(i+1), Height/6*PitchData[i+1]/30);
    
   // stroke(0,0,255);
    //line(Width/300*i, Height/6* kal_RollData[i]/30, Width/300*(i+1), Height/6* kal_RollData[i+1]/30);

    stroke(255,0,0);
    line(Width/300*i, Height/6* kal_PitchData[i]/30, Width/300*(i+1), Height/6* kal_PitchData[i+1]/30);
  }
  
  stroke(0);
  popMatrix();
}  



void serialEvent(Serial myPort){
  String data;
  
  data = myPort.readStringUntil('\n');  //改行で区切る
  data = trim(data);  //空白文字を削除
 
  //println(data);
   if((data != null))
   {
   
     s_data.setData(float(split(data,","))); //シリアルポートで受けたデータを"," 区切りしてそれぞれ格納
         
      }
  }
  
  
  
 void graphInit(float Width, float Height, int p_X, int p_Y,String name)
 {
   pushMatrix();
   noFill();
   stroke(100);
   strokeWeight(3);
   translate(p_X,p_Y);  //グラフの左上の位置
   rect(0, 0, Width, Height);

   strokeWeight(1);  // メモリ線の幅
  
  for (int i=1; i<6; i++)  // メモリ線の描画
  {
    line(0, Height/6*i, Width, Height/6*i);  // 縦軸
    line(Width/6*i, 0, Width/6*i, Height);  // 横軸
  }
  
  // メモリ値の描画
  stroke(0);
  fill(100);  // メモリ値の文字色
  
  // 縦軸のメモリ値
  textSize(Height*0.06);  // 文字の大きさ
  textAlign(RIGHT);  // 文字の座標指定位置
  
  int j = -90;  // 縦軸の目盛りの最低値
  
  for (int i=6; i>=0;i--)  // メモリ戦の描画
  {
    text(j, -width/3*0.02, Height/6*i);  // 文字の描画
    j = j + 30;  // 縦軸の目盛りの更新
  }
  
  pushMatrix(); 
  rotate(radians(-90));  // 座標軸を90度回転
  textAlign(CENTER);  // 文字の座標指定位置
  text(name, -Height/2, -width/3*0.2);  // 縦軸ラベル
  
  popMatrix(); 
  
  // 横軸のメモリ値
  textAlign(CENTER);  // 文字の座標指定位置
  text("now", Width, Height*1.08);  // 文字の描画
  j = -30;  // 横軸の目盛りの最低値
  for(int i=0; i<=5; i++)
  {
    text(j, Width/6*i, Height*1.08);  // 文字の描画
    j = j + 5;  // 横軸の目盛りの更新
  }
  
  text("time [s]", Width/2, Height*1.2);  // 横軸ラベルの描画
  
 }
