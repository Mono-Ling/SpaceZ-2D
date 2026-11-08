using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Exp.CCD
{
    public class InitialVelocity : MonoBehaviour
    {
        public Vector2 linearVelocity;
        public float angularVelocity;
        void Update()
        {
            if (Input.GetKeyDown(KeyCode.Space))
            {
                var body = GetComponent<Rigidbody2D_Exp_CCD>();
                if (body == null)
                    return;
                body.LinearVelocity = linearVelocity;
                body.AngularVelocity = angularVelocity;
            }
        }
    }
}