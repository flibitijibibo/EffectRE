#region License
/* FNA - XNA4 Reimplementation for Desktop Platforms
 * Copyright 2009-2014 Ethan Lee and the MonoGame Team
 *
 * Released under the Microsoft Public License.
 * See LICENSE for details.
 */
#endregion

namespace Microsoft.Xna.Framework.Graphics
{
	public interface IEffectFog
	{
		public Vector3 FogColor
		{
			get;
			set;
		}

		public bool FogEnabled
		{
			get;
			set;
		}

		public float FogEnd
		{
			get;
			set;
		}

		public float FogStart
		{
			get;
			set;
		}
	}
}
