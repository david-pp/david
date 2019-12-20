using UnityEngine;
using System.Collections;

public class Upgrade : MonoBehaviour
{

    public Gun gun;

    void OnTriggerEnter(Collider other)
    {
        gun.UpgradeGun();
        Destroy(gameObject);
        SoundManager.Instance.PlayOneShot(SoundManager.Instance.powerUpPickup);
    }
}