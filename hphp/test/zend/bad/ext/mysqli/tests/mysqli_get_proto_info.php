<?php
	require_once("connect.inc");
	require "table.inc";
	if (!is_int($info = mysqli_get_proto_info($link)) || ($info < 1))
		printf("[003] Expecting int/any_non_empty, got %s/%s\n", gettype($info), $info);
	print "done!";
?>