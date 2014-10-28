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
	public class Effect : GraphicsResource
	{
		#region Public Properties

		public EffectTechnique CurrentTechnique
		{
			get;
			set;
		}

		public EffectParameterCollection Parameters
		{
			get;
			private set;
		}

		public EffectTechniqueCollection Techniques
		{
			get;
			private set;
		}

		#endregion

		#region Public Constructor

		public Effect(GraphicsDevice graphicsDevice, byte[] effectCode)
		{
		}

		#endregion

		#region Protected Constructor

		protected Effect(Effect cloneSource)
		{
		}

		#endregion

		#region Public Methods

		public virtual Effect Clone()
		{
		}

		#endregion

		#region Protected Methods

		protected override void Dispose(bool disposing)
		{
		}

		protected internal virtual void OnApply()
		{
		}

		#endregion
	}
}
