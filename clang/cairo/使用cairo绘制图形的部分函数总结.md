使用cairo绘制图形的部分函数总结
====

一 绘制文本
```
    //设置源的颜色
    cairo_set_source_rgb(cr,0.627,0,0);
    //选择字体类型或尺寸
    cairo_select_font_face(cr,"Adobe Heiti Std",CAIRO_FONT_SLANT_NORMAL,CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr,24);

    //从图像区域的（40.0，64.0）开始绘制
    cairo_move_to(cr,40.0,64.0);
    //使用show_text
    cairo_show_text(cr,"Hello Cairo!");
```

二 绘制线段
```
    cairo_t *cr;

    //生成环境
    cr = gdk_cairo_create(widget->window);

    //设置线段颜色和粗细
    cairo_set_source_rgb(cr,0,0,0);
    cairo_set_line_width(cr,0.5);
    //count保存左键点击的次数，即数组内保存的点的坐标数
    //从第一个点开始，每个点依次与其它点绘制一条线段（实际上绘制了两条）
    //cairo_move_to和cairo_line_to在cr中定义了绘图路径
    int i,j;

    for(i = 0;i <= count-1;i++)
    {
           for(j = 0;j <= count-1;j++)
    		{
    			  cairo_move_to(cr,coordx[i],coordy[i]);
    						cairo_line_to(cr,coordx[j],coordy[j]);
    				}
    		}

    count = 0;
    //cairo_stroke将cr中的路径绘制出来（与显示文本的cairo_show_text不同）
    cairo_stroke(cr);
    cairo_destroy(cr);
```

三 绘制矩形
```
    cairo_t *cr;
    cr = gdk_cairo_create(widget->window);
    cairo_set_source_rgb(cr,0,0,0);
    cairo_set_line_width(cr,1);
    //绘制一个矩形，起点是20 20，长120，宽80
    cairo_rectangle(cr,20,20,120,80);
    //绘制一个正方形，是矩形的特例，长宽相等
    cairo_rectangle(cr,180,20,80,80);
    //绘制边线，且保留路径
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr,0,0,1);
    //填充，且清除路径
    cairo_fill(cr);
```

四 绘制圆形
```
    cairo_t *cr;
    cr = gdk_cairo_create(widget->window);
    int width,height;
            gtk_window_get_size(GTK_WINDOW(widget),&width,&height);
    cairo_set_line_width(cr,9);
    cairo_set_source_rgb(cr,0.69,0.19,0);
    //描绘圆的轮廓,圆心是width/2,height/2,半径为宽和高较小的那个的一半再减10
    //起始角为0，终止角为360
    //注意，这实际上是绘制一条弧线。
    cairo_arc(cr,width/2,height/2,(width < height ? width:height)/2 - 10,0,2 * M_PI);
    //cairo_stroke_preserve () 函数会将它绘制的路径依然保存在 cairo 环境中，
    //而 cairo_stroke () 所绘制的路径，在绘制完成后，就从 cairo的环境中清除了
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr,0.3,0.4,0.6);
    //对使用 cairo_stroke_preserve () 函数绘制的路径进行蓝色填充。
    //若上面使用cairo_stroke，则不填冲，因为stroke在绘制完路径后，就清除了，所以
    //fill无法填充。
    cairo_fill(cr);
    cairo_destroy(cr);
```

五 绘制弧线
```
    cairo_set_source_rgb(cr,0,0,0);
    //注意，这里的M_PI/4是指起点是M_PI/4的半径与弧的交点
    cairo_arc(cr,90,160,40,M_PI / 4,M_PI);
    //该圆弧并不是封闭的，只转过了180度，可以用close封闭它
    cairo_close_path(cr);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr,1,1,1);
    cairo_fill(cr);
```

六 绘制椭圆
```
    cairo_set_source_rgb(cr,0,0,0);
    //cairo_translate() 函数可通过平移用于空间的原点来修改当前的变换矩阵
    cairo_translate(cr,220,180);
    //设定椭圆长轴和短轴的比例
    cairo_scale(cr,1,0.7);
    cairo_arc(cr,200,180,50,0,2*M_PI);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr,1,1,1);
    cairo_fill(cr);
```
