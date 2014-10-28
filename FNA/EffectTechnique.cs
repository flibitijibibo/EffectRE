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
	public sealed class EffectTechnique
	{
		#region Public Properties

		public string Name
		{
			get;
			private set;
		}

		public EffectPassCollection Passes
		{
			get;
			private set;
		}

		public EffectAnnotationCollection Annotations
		{
			get;
			private set;
		}

		#endregion
	}
}
