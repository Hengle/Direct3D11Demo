
<!-- @import "[TOC]" {cmd="toc" depthFrom=1 depthTo=6 orderedList=false} -->

<!-- code_chunk_output -->

- [Direct3D11 Demo](#direct3d11-demo)
- [实际截图](#实际截图)
  - [DXUT](#dxut)
  - [DynamicShaderlinkage](#dynamicshaderlinkage)
- [MultithreadedRendering](#multithreadedrendering)
- [Predicated](#predicated)

<!-- /code_chunk_output -->


# Direct3D11 Demo

主要是学习并编写DirectSDK sample中的例子

- 使用的框架为：DXUT+ImGui+Effect11
- 已学习的部分：
  - DXUT Tutorials          (DXUT框架，基本着色器)
  - DynamicShaderlinkage    (动态着色器链接)
  - MultithreadedRendering  (多线程渲染)
  - Predicated              (d3d预测属性)


# 实际截图

## DXUT

dxut是一个封装的D3D的程序框架，简化的d3d API的使用，直接提供创建窗口设备，处理窗机消息，控制设备等功能

![Multithreaded](/resource/basefx.gif)

## DynamicShaderlinkage

动态着色器链接可以设计通用着色器框架，避免由于复杂的需求带来的代码复杂度或过多的宏定义


# MultithreadedRendering

使用多线程+deferred context,实现以view为划分的多线程渲染。
线程数 = deferred context数 = 目标视图总和 = shadow + dynamicSkybox + 1
deferred context的command list最终由immediate context提交至GPU

![Multithreaded](/resource/multiThread.gif)

# Predicated

预测是一种基于前一种调用结果决定下一次处理结果
D3D11_QUERY_OCCLUSION_PREDICATE 是一种仅使用于GPU的遮挡预测，GPU可以根据设置的predicate结果来决定本次是否进行绘制调用。
例如一个相同的模型的高模和低模，先用低模进行遮挡预测，然后使用GPU通过预测结果来决定高模是否进行drawcall，这种方式比常规的可见性测试节省了渲染管线前期的调用计算时间。

![Predicated](/resource/predicated.gif)

# PBR

使用基于物理的光照模型进行渲染，对物体表面进行建模，入射光的落点做一个半圆，捕获面积和光辐射信息，利用半球的立体角计算微元投影面积。并收集半球上的光照与材质的交互结果

- 自发光

  物体本身发出的光

  ![emissions](/resource/emission.PNG)

- BRDF
  双向反射分布函数，描述入射光经过表面后如何在出射方向上分布的结果
  - 漫反射

    漫反射的反射光就是材质本身的颜色。由于一个入射点是一个半球，因此遵循能量守恒，入射光会被均匀的散射到半球的微元，因此diffuseTerm = diffuse/pi

  - 法线分布函数(Trowbridge-Reitz GGX)

    基于粗糙度和半角向量与法线夹角，计算物体表面的能够提供反射的微平面(其中微法线与宏观法线相等的平面就是活跃的微平面),提供高光反射的计算

    ![法线分布](/resource/normaldistribution.PNG)

  - 菲涅尔系数(SchlickFresnel)

    反应一个材质的反射和折射光量，反应到公式就是漫反射和镜面反射。F0是物体表面的固有属性反应了一个表面的反射比例，折射的固有属性为(1-F0).
    通常F0 = specular  
    ![Fresnel](/resource/fresnel.PNG)

  - 可见性函数(阴影遮挡函数,几何函数)
    在光线入射到出射的过程中，微平面自身可能被其他微平挡住导致入射或出射光线被挡住。可见性函数就是描述由多少比例的微表面被遮挡。  
    
    公式类似法线分布，依据表面粗糙度，视线和法线的角度，入射光和发现的角度计算被遮挡的微表面的比例。  
    大概表现为，入射光与法线角度越大，粗糙度越高，遮蔽率越大。出射光同样如此  
    ![可见性](/resource/Visibility.PNG)

- 金属工作流

  其中金属的反射率很高，非金属的反射率非常低。  
  也就是说金属对光线基本都是反射，而折射的光线则被吸收掉。金属表现出来效果是由反射光分量决定的.展现到公式上就是diffuse = 0,specular = baseColor

  非金属则会将折射的光经过散射重新发射出来。而反射分量则比较低。

  由于金属和非金属的特性，因此可以使用一个标量(metalness)来代替传统的specular和diffuse两个RGB颜色分量.  
  其中计算公式为  
  diffuse = baseColor *(1-Metalness)  
  specular= baseColor* Metalness + (1-Metalness)*baseColor  
  绝缘体的反射光量只有(0.04,0.04,0.04),金属的反射光量为baseColor，而diffuse=0.半导体介于两者之间

  ![漫反射](/resource/diffuse.PNG)  
  ![高光反射](/resource/specular.PNG)

- 基于图像的光照(IBL)
  
  将场景存储在环境贴图上，环境贴图可以在光滑的物体表面发生反射，通过mipmap机制可以实现根据材质粗糙度获取不同反射结果。粗糙度越大，mipmap等级越高，图像分辨率越小，反射图像越模糊

  ![IBL](/resource/IBL.gif) 

- 最终效果

![final](/resource/final.gif)  
    



