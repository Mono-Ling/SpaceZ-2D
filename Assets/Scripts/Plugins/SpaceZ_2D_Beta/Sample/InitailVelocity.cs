using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace SpaceZ_2D.Beta
{
    public class InitailVelocity : MonoBehaviour
    {
        public Vector2 linearVelocity;
        public float angularVelocity;
        private Rigidbody2D_Z _body;
        // Start is called before the first frame update
        void Start()
        => _body = GetComponent<Rigidbody2D_Z>();

        // Update is called once per frame
        void Update()
        {
            if(_body == null)
                return;
            if(Input.GetKeyDown(KeyCode.Space))
            {
                _body.LinearVelocity = linearVelocity;
                _body.AngularVelocity = angularVelocity;
            }
        }
    }
}