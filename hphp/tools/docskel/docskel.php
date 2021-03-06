<?hh
include(__DIR__ . '/base.php');

function generateDocComment(string $doccomment, array $func = null,
                            string $indent = ''): string {
  $str = $doccomment . "\n";
  if (!empty($func['args'])) {
    $str .= "\n";
    foreach ($func['args'] as $arg) {
      $desc = isset($arg['desc']) ? $arg['desc'] : '';
      $v = "@param {$arg['type']} \${$arg['name']} - " .
           str_replace("\n", " ", $desc);
      $str .= implode("\n  ", explode("\n",
                      wordwrap($v, 70 - strlen($indent)))) . "\n";
    }
  }
  if (!empty($func['return'])) {
    $str .= "\n";
    $v = "@return {$func['return']['type']} - ";
    $v .= isset($func['return']['desc'])
       ? str_replace("\n", " ", $func['return']['desc']) . "\n"
       : '';
    $str .= implode("\n  ", explode("\n",
                    wordwrap($v, 70 - strlen($indent)))) . "\n";
  }
  $str = trim($str);
  if (empty($str)) return '';

  $block = wordwrap(trim($str), 75 - strlen($indent));
  return "$indent/**\n".
         "$indent * ".
         str_replace(["\n"," \n"], ["\n$indent * ","\n"], $block).
         "\n$indent */\n";
}

function generateFunctionSignature(array $func, string $indent = ''): string {
  $modifiers = empty($func['modifiers'])
             ? '' : (implode(' ', $func['modifiers']) . ' ');
  $ret = "$indent{$modifiers}function {$func['name']}(";
  $argspace = ",\n" . str_repeat(' ', strlen($ret));
  $notfirst = false;
  foreach ($func['args'] as $arg) {
    if ($notfirst) $ret .= $argspace;
    $notfirst = true;
    if (empty($arg['type'])) {
      $ret .= "mixed";
    } else {
      $ret .= $arg['type'];
    }
    $ret .= ' ';
    if ($arg['reference']) {
      $ret .= '&';
    }
    $ret .= "\${$arg['name']}";
    if (!empty($arg['default'])) {
      $ret .= " = {$arg['default']}";
    }
  }
  $ret .= '): ' . (empty($func['return']['type']) ? 'void'
                                                  : $func['return']['type']);
  return "$indent<<__Native>>\n$ret;\n\n";
}

function outputSystemlib(string $dest, array $funcs, array $classes):void {
  $fp = fopen($dest, 'w');
  fwrite($fp, "<?hh\n");
  fwrite($fp, "// @"."generated by docskel.php\n\n");
  foreach($classes as $class) {
    if (!empty($class['intro'])) {
      fwrite($fp, generateDocComment($class['intro']));
    }
    fwrite($fp, "class {$class['name']}");
    if (!empty($class['extends'])) {
      fwrite($fp, " extends {$class['extends']}");
    }
    if (!empty($class['implements'])) {
      fwrite($fp, " implements " . implode(', ', $class['implements']));
    }
    fwrite($fp, " {\n");
    if (!empty($class['functions'])) {
      foreach($class['functions'] as $func) {
        fwrite($fp, generateDocComment($func['desc'], $func, '  '));
        fwrite($fp, generateFunctionSignature($func, '  '));
      }
    }
    fwrite($fp, "}\n\n");
  }

  foreach($funcs as $func) {
    fwrite($fp, generateDocComment($func['desc'], $func));
    fwrite($fp, generateFunctionSignature($func));
  }
}

function getMethod(string $name, array $classes): array {
  $colon = strpos($name, '::');
  if ($colon ===  false) {
    return array();
  }

  $cname = strtolower(substr($name, 0, $colon));
  $mname = strtolower(substr($name, $colon + 2));
  if (isset($classes[$cname]['functions'][$mname])) {
    return $classes[$cname]['functions'][$mname];
  }
  return array();
}

function generateCPPStub(array $func, array $classes): string {
  static $typemap = [
    'bool' => ['bool', 'bool'],
    'int' => ['int64_t', 'int64_t'],
    'float' => ['double', 'double'],
    'string' => ['const String&', 'String'],
    'array' => ['CArrRef', 'Array'],
    'object' => ['CObjRef', 'Object'],
    'resource' => ['CResRef', 'Resource'],
    'mixed' => ['CVarRef', 'Variant'],
    'void' => [1 =>'void'],
  ];

  $ret = 'static ';
  if (empty($func['return']['type'])) {
    $ret .= 'void';
  } elseif (isset($typemap[$func['return']['type']])) {
    $ret .= $typemap[$func['return']['type']][1];
  } else {
    $ret .= 'Object';
  }
  if (empty($func['class'])) {
    $ret .= " HHVM_FUNCTION({$func['name']}";
  } else {
    $type = in_array('static', $func['modifiers'])
          ? 'HHVM_STATIC_METHOD' : 'HHVM_METHOD';
    $ret .= " $type({$func['class']}, {$func['name']}";
  }
  foreach($func['args'] as $arg) {
    if ($arg['reference']) {
      $ret .= ', VRefParam';
    } elseif (isset($typemap[$arg['type']])) {
      $ret .= ', ' . $typemap[$arg['type']][0];
    } else {
      $ret .= ', CObjRef';
    }
    $ret .= " {$arg['name']}";
  }
  if (!empty($func['class']) || empty($func['alias']) ||
      !($method = getMethod($func['alias'], $classes))) {
    return "$ret) {\n  throw NotImplementedException(\"Not Implemented\");".
           "\n}\n\n";
  }
  $ret .= ") {\n  ";
  if (!empty($func['return']['type']) &&
      ($func['return']['type'] != 'void')) {
    $ret .= 'return ';
  }
  $comma = false;
  if (in_array('static', $method['modifiers'])) {
    $ret .= "HHVM_STATIC_MN({$method['class']}, {$method['name']})".
            "(Unit::lookupClass(s_{$method['class']}.get())";
    $comma = true;
  } else {
    $ret .= "HHVM_MN({$method['class']}, {$method['name']})(";
  }

  foreach($func['args'] as $arg) {
    if ($comma) $ret .= ', ';
    $comma = true;
    $ret .= $arg['name'];
  }

  return "$ret);\n}\n\n";
}

function outputExtensionCPP(string $dest, string $extname,
                            array $funcs, array $classes): void {
  $fp = fopen($dest, 'w');
  fwrite($fp, "#include \"hphp/runtime/base/base-includes.h\"\n");
  fwrite($fp, "namespace HPHP {\n");

  foreach($classes as $class) {
    fwrite($fp, "const StaticString s_{$class['name']}".
                "(\"{$class['name']}\");\n");
    if (empty($class['functions'])) continue;
    fwrite($fp, str_repeat('/', 78) . "\n// class {$class['name']}\n\n");
    foreach($class['functions'] as $func) {
      fwrite($fp, generateCPPStub($func, $classes));
    }
  }

  if (!empty($funcs)) {
    fwrite($fp, str_repeat('/', 78) . "\n// functions\n\n");
    foreach($funcs as $func) {
      fwrite($fp, generateCPPStub($func, $classes));
    }
  }

  fwrite($fp, str_repeat('/', 78) . "\n\n");
  fwrite($fp, "class {$extname}Extension : public Extension {\n");
  fwrite($fp, " public:\n");
  fwrite($fp, "  {$extname}Extension() : Extension(\"{$extname}\") {}\n");
  fwrite($fp, "  virtual void moduleInit() {\n");

  foreach($classes as $class) {
    if (empty($class['functions'])) continue;
    foreach($class['functions'] as $func) {
      $type = in_array('static', $func['modifiers'])
            ? 'HHVM_STATIC_ME' : 'HHVM_ME';
      fwrite($fp, "    $type({$func['class']}, {$func['name']});\n");
    }
  }

  foreach($funcs as $func) {
    if (empty($func['name'])) continue;
    fwrite($fp, "    HHVM_FE({$func['name']});\n");
  }

  fwrite($fp, "    loadSystemlib();\n");
  fwrite($fp, "  }\n");
  fwrite($fp, "} s_{$extname}_extension;\n\n");
  fwrite($fp, "// Uncomment for non-bundled module\n");
  fwrite($fp, "//HHVM_GET_MODULE(${extname});\n\n");
  fwrite($fp, str_repeat('/', 78) . "\n");
  fwrite($fp, "} // namespace HPHP\n");
}

if (empty($_SERVER['argv'][2])) {
  fwrite(STDERR, "Usage: {$_SERVER['argv'][0]} <phpdoc-root> <extname> ".
                 "[ exname.cpp [ extname.php ] ]\n");
  exit;
}

$extname = $_SERVER['argv'][2];
$ext = new HHVMDocExtension($extname, $_SERVER['argv'][1]);
$cppfile = empty($_SERVER['argv'][3])
         ? "$extname.cpp" : $_SERVER['argv'][3];
$phpfile = empty($_SERVER['argv'][4])
         ? "$extname.php" : $_SERVER['argv'][4];

if (!preg_match('@^[a-zA-Z0-9_\.-]+$@', $extname)) {
  die("Invalid extension name: $extname\n");
}
$ext->setVerbose(true);

outputSystemlib($phpfile,
                $ext->getFunctions(),
                $ext->getClasses());
outputExtensionCPP($cppfile, $extname,
                   $ext->getFunctions(),
                   $ext->getClasses());
