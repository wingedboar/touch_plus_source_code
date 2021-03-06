/*
 * Touch+ Software
 * Copyright (C) 2015
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Aladdin Free Public License as
 * published by the Aladdin Enterprises, either version 9 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Aladdin Free Public License for more details.
 *
 * You should have received a copy of the Aladdin Free Public License
 * along with this program.  If not, see <http://ghostscript.com/doc/8.54/Public.htm>.
 */

package
{
	import flash.external.*;

	public class Interop
	{
		private static var cb_count:int = 0;

		public static function call_js(js_code, cb):void
		{
			var cb_name:String = "cb" + cb_count.toString();
			ExternalInterface.addCallback(cb_name, cb);
			++cb_count;
			ExternalInterface.call("gui." + cb_name + "(" + js_code + ")");
		}

		public static function log(content):void
		{
			ExternalInterface.call("console.log('" + content + "')");
		}
	}
}