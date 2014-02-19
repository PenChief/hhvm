<?php


class MyClas
{
  public function __construct() {
    
  }
  
  public function foo() {
    echo __function__,__file__,"\n";
  }

  public function bar() {
    bla();
  }
  public function all() {
    $this->bar();
  }
};


function MyMain($name, &$otehrParam)
{
  echo $name . "\n";
  $a = new MyClas();
  $a->all();
}

function yonMan() {
  echo __function__,__file__,"\n";
}

$bb = new MyClas();
$bb->foo();

// MyMain
$otehrParam = "Stam Value";
MyMain( "Hello World", $otehrParam );
