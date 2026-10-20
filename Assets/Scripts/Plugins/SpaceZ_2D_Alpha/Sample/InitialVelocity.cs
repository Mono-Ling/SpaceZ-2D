using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Alpha
{
    public class InitialVelocity : MonoBehaviour
    {
        public Vector2 linearVelocity;
        public float angularVelocity;
        // Start is called before the first frame update
        void Start()
        {
            var body = GetComponent<Rigidbody2DAlpha>();
            if(body == null)
            {
                Debug.LogError($"【InitialVelocity】{typeof(Rigidbody2DAlpha).Name}获取失败");
                return;
            }
            body.LinearVelocity = linearVelocity;
            body.AngularVelocity = angularVelocity;
        }
    }
}