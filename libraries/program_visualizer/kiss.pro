<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="100" height="50" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>FlowchartItem for KISS-C Program Visualizer
  </desc>
  <defs>

<rect id="boxall" fill="aquamarine" stroke="black" stroke-width="1" x="2" y="0" width="96" height="50" rx="12"/>

<g id="boxleft">
<path fill="aquamarine" stroke="black" stroke-width="1" d="M 16 48 L 14 48 A 12 12 0 0 1 2 36 L 2 14 A 12 12 0 0 1 14 2 L 16 2"/>
</g>

<g id="boxright" transform="rotate(180, 8, 25)">
<use xlink:href="#boxleft"/>
</g>

<g id="boxmiddle">
<rect fill="aquamarine" stroke="none" x="0" y="2" width="50" height="46"/>
<line stroke="black" stroke-width="1" x1="0" y1="2" x2="50" y2="2"/>
<line stroke="black" stroke-width="1" x1="0" y1="48" x2="50" y2="48"/>
</g>

<!--

<clipPath id="boxleftclip">
<rect x="0" y="0" width="16" height="50"/>
</clipPath>

<g id="boxleft" clip-path="url(#boxleftclip)">
<use xlink:href="#boxall"/>
</g>

<g id="boxright" clip-path="url(#boxleftclip)">
<g transform="translate(-84,0)">
<use xlink:href="#boxall"/>
</g>
</g>

<clipPath id="boxmiddleclip">
<rect x="0" y="0" width="50" height="50"/>
</clipPath>

<g id="boxmiddle" clip-path="url(#boxmiddleclip)">
<g transform="translate(-25,0)">
<use xlink:href="#boxall"/>
</g>
</g>
  
-->
  

  
  </defs>
  
<use xlink:href="#boxmiddle"/>

</svg>

