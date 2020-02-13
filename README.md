![Redopera](/icon/Redopera.png "logo")  

# Redopera 基于OpenGL的C++多线程2D游戏开发库

  * Redopera使用一个同步事件循环
  * 使用Redopera开发游戏的流程为：继承一个Conrtoller类辅以工具类管理资源类。
  * Redopera的大部分功能模块来源于其他开源项目，以统一实用为目标进行封装。
  * 模块之间尽量避免依赖，在需要的时候可以很容易的进行替换。

---

## 目录

  * [模块](#模块)
  * [部署](#部署)
  * [更新](#更新)
  * [依赖](#依赖)
  * [许可](#许可)

---

## 模块

<table style="font-size:14px">
   <tr>
      <th colspan="2">模块</th>
      <th>功能</th>
      <th>文件</th>
      <th>依赖</th>
   </tr>
   <tr>
      <td rowspan="3">控制</td>
      <td></td>
      <td>控制循环或循环中的行为</td>
      <td>RController.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="2" style="text-align:center">窗口</td>
      <td>窗口控制</td>
      <td>RWindowCtrl.h</td>
      <td>GLFW、GLAD</td>
   </tr>
   <tr>
      <td>信息窗口</td>
      <td>RInfoWindow.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="21">工具</td>
      <td rowspan="4" style="text-align:center">IO</td>
      <td>音频播放</td>
      <td>RAudioStream.h</td>
      <td>RtAudio</td>
   </tr>
   <tr>
      <td>Debug相关</td>
      <td>RDebug.h</td>
      <td></td>
   </tr>
   <tr>
      <td>鼠标、键盘与手柄输入管理</td>
      <td>RInputModule.h</td>
      <td>GLFW</td>
   </tr>
   <tr>
      <td>事件通知发布</td>
      <td>RNotifyEvent.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="6" style="text-align:center">数据</td>
      <td>RGBA色值</td>
      <td>RColor.h</td>
      <td></td>
   </tr>
   <tr>
      <td>解析JSON</td>
      <td>RJson.h</td>
      <td>json</td>
   </tr>
   <tr>
      <td>OpenGL数学</td>
      <td>RMaths.h</td>
      <td>GLM</td>
   </tr>
   <tr>
      <td>二维与三维点</td>
      <td>RPoint.h</td>
      <td></td>
   </tr>
   <tr>
      <td>平面信息</td>
      <td>RRect.h</td>
      <td></td>
   </tr>
   <tr>
      <td>尺寸信息</td>
      <td>RSize.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="4" style="text-align:center">渲染</td>
      <td>顺序GLFW、GLAD的头文件</td>
      <td>ROpenGL.h</td>
      <td>GLFW、GLAD</td>
   </tr>
   <tr>
      <td>渲染平面</td>
      <td>RPlane.h</td>
      <td>GLAD</td>
   </tr>
   <tr>
      <td>渲染精灵</td>
      <td>RSprite.h</td>
      <td>GLAD</td>
   </tr>
   <tr>
      <td>文字平面</td>
      <td>RTextbox.h</td>
      <td>GLAD</td>
   </tr>
   <tr>
      <td></td>
      <td>信号槽</td>
      <td>RSigslot.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="4" style="text-align:center">多线程</td>
      <td>线程</td>
      <td>RThread.h</td>
      <td></td>
   </tr>
   <tr>
      <td>线程池</td>
      <td>RThreadPool.h</td>
      <td></td>
   </tr>
   <tr>
      <td>线程安全队列</td>
      <td>RThreadQueue.h</td>
      <td></td>
   </tr>
   <tr>
      <td>线程安全堆栈</td>
      <td>RThreadStack.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="2" style="text-align:center">时间</td>
      <td>时钟</td>
      <td>RTime.h</td>
      <td></td>
   </tr>
   <tr>
      <td>定时器</td>
      <td>RTimer.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="11" style="text-align:center">资源</td>
      <td></td>
      <td>资源类基类</td>
      <td>RResource.h</td>
      <td></td>
   </tr>
   <tr>
      <td rowspan="7" style="text-align:center">文件加载</td>
      <td>光标</td>
      <td>RCursor.h</td>
      <td>GLFW</td>
   </tr>
   <tr>
      <td>字体</td>
      <td>RFont.h</td>
      <td>stb_truetype</td>
   </tr>
   <tr>
      <td>图像</td>
      <td>RImage.h</td>
      <td>std_image</td>
   </tr>
   <tr>
      <td>Lua脚本</td>
      <td>RLuaScript.h</td>
      <td>Lua</td>
   </tr>
   <tr>
      <td>数据包文件</td>
      <td>RPack.h</td>
      <td></td>
   </tr>
   <tr>
      <td>mp3文件</td>
      <td>RMp3.h</td>
      <td>minimp3</td>
   </tr>
   <tr>
      <td>着色器</td>
      <td>RShader.h</td>
      <td>GLAD</td>
   </tr>
   <tr>
      <td rowspan="2" style="text-align:center">OpenGL</td>
      <td>着色器程序</td>
      <td>RShaderProg.h</td>
      <td>GLAD</td>
   </tr>
   <tr>
      <td>纹理</td>
      <td>RTexture.h</td>
      <td>GLAD</td>
   </tr>
   <tr>
      <td style="text-align:center">字体</td>
      <td>内嵌字体</td>
      <td>SourceCodePro.cpp</td>
      <td>SourceCodePro</td>
   </tr>
</table>

---

## 部署
  * pass

---

## 更新

### 发布 1.0.0 Alpha版本 —— 2020.02.13
  * 初步完成基础功能，但未经稳定测试的版本

---

## 依赖

  * [GLFW](https://www.glfw.org/) v3.4  
  Copyright (c) 2002-2006 Marcus Geelnard  
  Copyright (c) 2006-2019 Camilla Löwy <elmindreda@glfw.org>  
  [License](extern/LICENSE_GLFW.md): zlib/libpng

  * [GLAD](https://github.com/Dav1dde/glad)  
  Copyright (c) 2013-2018 David Herberth  
  [License](extern/LICENSE_GLAD.md): MIT

  * [RtAudio](http://www.music.mcgill.ca/~gary/rtaudio) v5.1.0  
  Copyright (c) 2001-2019 Gary P. Scavone  
  [License](extern/LICENSE_RtAudio.md): ...

  * [json](https://github.com/nlohmann/json) v3.7.3  
  Copyright (c) 2013-2019 Niels Lohmann <http://nlohmann.me>  
  [License](extern/LICENSE_json.md): MIT

  * [GLM](https://github.com/g-truc/glm) v4.2  
  Copyright (c) 2005 - G-Truc Creation  
  [License](extern/LICENSE_GLM.md): Happy Bunny & MIT

  * [stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h) v1.22  
  Copyright (c) 2017 Sean Barrett  
  [License](extern/LICENSE_stb.md): MIT & Public Domain

  * [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) v2.23  
  Copyright (c) 2017 Sean Barrett  
  [License](extern/LICENSE_stb.md): MIT & Public Domain

  * [Lua](http://www.lua.org/) v5.3.5  
  Copyright © 1994–2019 Lua.org, PUC-Rio.   
  [License](extern/LICENSE_Lua.md): ...

  * [minimp3](https://github.com/lieff/minimp3)  
  Copyright © ...   
  [License](extern/LICENSE_minimp3.md): CC0 1.0 Universal

  * [Source Code Pro](https://github.com/lieff/minimp3)  
  Copyright 2010-2019 Adobe (http://www.adobe.com/)   
  [License](extern/LICENSE_source-code-pro.md): SIL Open Font

---

## 许可
  Copyright (c) 2020 chilingg  
  License: [MIT](LICENSE.md)
