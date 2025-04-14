// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

interface IAncFeature {
    ChangeAnc()
    GetAnc()
}

interface IFeature {
    string GetFeatureName(); // "battery"
    bool IsReadOnly(); // true
}

interface IBooleanFeature : IFeature {
    bool GetCurrentValue();
    void SetValue(bool value)
}

interface IComboFeature : IFeature { // ANC, 
    List<string> GetOptions();
    string GetCurrentValue();
    void SetCurrentValue(string value);
}

interface IDevice { 
    List<Feature> GetFeatures() // 3 IBooleanFeature
}




foreach (var feature in GetFeatures()) {
    if (feature is IBooleanFeature boolenFeature) {
        CreateJson {
            feature.GetFeatureName()
            feature.
        }
    }
}



Уровень заряда  (ReadOnly)
Шумодав IComboFeature (ReadWrite)

client -> bytes -> AppleSdk -> единый стандарт -> websocketclient -> json -> ui
client -> bytes -> SamsungSdk -> единый стандарт -> websocketclient -> json -> ui


client -> bytes -> Sdk -> json -> websocket -> ui
