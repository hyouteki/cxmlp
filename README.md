> C XML (eXtensible Markup Language) Parser

## Getting Started

``` bash
git submodule update --init --recursive
```

## Test

``` bash
make
```

``` xml
<row id="main">
  <column id="sidebar" background="black">
    <image id="logo" src="./resources/logo.png"/> 
    <button id="button1" text="Menu Button 1"/>
    <button id="button2" text="Menu Button 2"/>
    <label id="menu label" text="Sample Label with word wrap"/>
  </column>
  <column id="canvas" background="dark brown">
    <button
        id="button-fixed"
        text="Fixed button inside canvas"
        fixed="true"/>
    <button id="button-canvas" text="Button inside canvas"/>
    <label id="lorem" style="heading" text="Lorem Ipsum"/>
    <label id="lorem-text" lorem="true"/>
  </column>
</row>
```

## Dependency

- [steel - nos1dot618](https://github.com/nos1dot618/steel): STL (standard template library) for C.
