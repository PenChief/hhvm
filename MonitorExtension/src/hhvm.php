<?php


class MyClas {
public function foo() {
	echo "foo\n";
}

public function bar() {
	echo "bar\n";
}
public function all() {
	bla();
}
};

function amit() {
	$a = new MyClas();	
	$a->all();
}

amit();


